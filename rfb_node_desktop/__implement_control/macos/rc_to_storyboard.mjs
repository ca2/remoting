import fs from "node:fs";
import path from "node:path";

const sourceDir = path.resolve(path.dirname(new URL(import.meta.url).pathname), "..");
const rcPath = path.join(sourceDir, "main.rc");
const resourcePath = path.join(sourceDir, "resource.h");
const storyboardPath = path.join(sourceDir, "macos", "Storyboard.storyboard");
const resourceNamesPath = path.join(sourceDir, "macos", "resource_names.inl");

const rc = fs.readFileSync(rcPath).toString("utf16le").replace(/^\uFEFF/, "");
const resource = fs.readFileSync(resourcePath, "utf8");
const standardResourceIds = new Map([
   ["IDOK", 1],
   ["IDCANCEL", 2],
]);
const declaredResourceIds = new Map(
   [...resource.matchAll(/^#define\s+(\w+)\s+(-?\d+)/gm)]
      .map((match) => [match[1], Number(match[2])]),
);
const resourceIds = new Map([...standardResourceIds, ...declaredResourceIds]);

let serial = 0;
const makeId = (prefix) => `${prefix}-${String(++serial).padStart(4, "0")}`;
const xml = (value = "") => String(value)
   .replaceAll("&", "&amp;")
   .replaceAll('"', "&quot;")
   .replaceAll("<", "&lt;")
   .replaceAll(">", "&gt;");
const number = (value, fallback = 0) => Number.parseFloat(value) || fallback;
const tag = (symbol) => {
   const value = resourceIds.get(symbol);
   return Number.isFinite(value) && value >= 0 ? ` tag="${value}"` : "";
};

function parseDialogs(text) {
   const lines = text.split(/\r?\n/);
   const dialogs = [];

   for (let index = 0; index < lines.length; index++) {
      const header = lines[index].match(/^(IDD_\w+)\s+DIALOGEX\s+(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+)/);
      if (!header) continue;

      const dialog = {
         id: header[1],
         width: number(header[4]),
         height: number(header[5]),
         title: header[1].replace(/^IDD_/, "").replaceAll("_", " "),
         controls: [],
      };

      while (++index < lines.length && !/^BEGIN\s*$/.test(lines[index])) {
         const caption = lines[index].match(/^CAPTION\s+"(.*)"\s*$/);
         if (caption) dialog.title = caption[1].replaceAll('""', '"');
      }

      let statement = "";
      while (++index < lines.length && !/^END\s*$/.test(lines[index])) {
         const trimmed = lines[index].trim();
         if (!trimmed) continue;
         statement += `${statement ? " " : ""}${trimmed}`;
         if (/,$/.test(trimmed)) continue;
         const control = parseControl(statement);
         if (control) dialog.controls.push(control);
         statement = "";
      }

      dialogs.push(dialog);
   }

   return dialogs;
}

function parseControl(statement) {
   const quoted = statement.match(/^(\w+)\s+"((?:""|[^"])*)",(\w+),(.*)$/);
   const unquoted = statement.match(/^(\w+)\s+(\w+),(.*)$/);
   if (!quoted && !unquoted) return null;

   const kind = (quoted || unquoted)[1];
   const title = quoted ? quoted[2].replaceAll('""', '"') : "";
   const identifier = quoted ? quoted[3] : unquoted[2];
   const tail = quoted ? quoted[4] : unquoted[3];
   const coordinates = [...tail.matchAll(/(?:^|,)\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(\d+)\s*,\s*(\d+)(?:,|$)/g)].at(-1);
   if (!coordinates) return null;

   return {
      kind,
      title,
      identifier,
      style: tail,
      x: number(coordinates[1]),
      y: number(coordinates[2]),
      width: number(coordinates[3]),
      height: number(coordinates[4]),
   };
}

const SX = 1.65;
const SY = 1.85;
const MIN_WIDTH = 340;
const MIN_HEIGHT = 150;

function frame(control, contentHeight) {
   const x = Math.round(control.x * SX);
   const width = Math.max(Math.round(control.width * SX), 18);
   const height = Math.max(Math.round(control.height * SY), 18);
   const y = Math.round(contentHeight - (control.y * SY) - height);
   return { x, y, width, height };
}

function rect(value) {
   return `<rect key="frame" x="${value.x}" y="${value.y}" width="${value.width}" height="${value.height}"/>`;
}

function controlXml(control, contentHeight, dialogId) {
   const id = makeId("ctl");
   const cellId = makeId("cell");
   const controlFrame = frame(control, contentHeight);
   const attributes = `identifier="${xml(`${dialogId}.${control.identifier}.${id}`)}"${tag(control.identifier)} fixedFrame="YES" translatesAutoresizingMaskIntoConstraints="NO" id="${id}" userLabel="${xml(control.identifier)}"`;
   const autoresizing = '<autoresizingMask key="autoresizingMask" flexibleMaxX="YES" flexibleMinY="YES"/>';

   if (control.kind === "LTEXT" || control.kind === "CTEXT" || control.kind === "RTEXT") {
      return `<textField ${attributes}>${rect(controlFrame)}${autoresizing}<textFieldCell key="cell" lineBreakMode="wordWrapping" title="${xml(control.title)}" id="${cellId}"><font key="font" metaFont="system"/><color key="textColor" name="labelColor" catalog="System" colorSpace="catalog"/><color key="backgroundColor" name="textBackgroundColor" catalog="System" colorSpace="catalog"/></textFieldCell></textField>`;
   }

   if (control.kind === "EDITTEXT") {
      const secure = control.style.includes("ES_PASSWORD");
      const element = secure ? "secureTextField" : "textField";
      const cell = secure ? "secureTextFieldCell" : "textFieldCell";
      return `<${element} ${attributes}>${rect(controlFrame)}${autoresizing}<${cell} key="cell" scrollable="YES" lineBreakMode="clipping" selectable="YES" editable="YES" sendsActionOnEndEditing="YES" borderStyle="bezel" drawsBackground="YES" id="${cellId}"><font key="font" metaFont="system"/><color key="textColor" name="controlTextColor" catalog="System" colorSpace="catalog"/><color key="backgroundColor" name="textBackgroundColor" catalog="System" colorSpace="catalog"/></${cell}></${element}>`;
   }

   if (control.kind === "PUSHBUTTON" || control.kind === "DEFPUSHBUTTON") {
      return `<button ${attributes}>${rect(controlFrame)}${autoresizing}<buttonCell key="cell" type="push" title="${xml(control.title)}" bezelStyle="rounded" alignment="center" borderStyle="border" imageScaling="proportionallyDown" inset="2" id="${cellId}"><behavior key="behavior" pushIn="YES" lightByBackground="YES" lightByGray="YES"/><font key="font" metaFont="system"/></buttonCell></button>`;
   }

   if (control.kind === "RADIOBUTTON"
      || control.style.includes("BS_AUTORADIOBUTTON")
      || control.style.includes("BS_RADIOBUTTON")) {
      return `<button ${attributes}>${rect(controlFrame)}${autoresizing}<buttonCell key="cell" type="radio" title="${xml(control.title)}" bezelStyle="regularSquare" imagePosition="left" alignment="left" inset="2" id="${cellId}"><behavior key="behavior" changeContents="YES" doesNotDimImage="YES" lightByContents="YES"/><font key="font" metaFont="system"/></buttonCell></button>`;
   }

   if (control.kind === "GROUPBOX") {
      return `<box ${attributes} title="${xml(control.title)}" boxType="primary">${rect(controlFrame)}${autoresizing}<view key="contentView" id="${makeId("box")}"><rect key="frame" x="3" y="3" width="${Math.max(controlFrame.width - 6, 12)}" height="${Math.max(controlFrame.height - 22, 12)}"/><autoresizingMask key="autoresizingMask"/></view></box>`;
   }

   if (control.kind === "COMBOBOX") {
      return `<comboBox ${attributes}>${rect(controlFrame)}${autoresizing}<comboBoxCell key="cell" scrollable="YES" lineBreakMode="clipping" selectable="YES" editable="YES" borderStyle="bezel" drawsBackground="YES" completes="NO" numberOfVisibleItems="8" id="${cellId}"><font key="font" metaFont="system"/><color key="textColor" name="controlTextColor" catalog="System" colorSpace="catalog"/><color key="backgroundColor" name="textBackgroundColor" catalog="System" colorSpace="catalog"/></comboBoxCell></comboBox>`;
   }

   if (control.kind === "LISTBOX" || control.style.includes("SysListView32")) {
      return `<scrollView ${attributes} borderType="bezel" hasVerticalScroller="YES">${rect(controlFrame)}${autoresizing}<clipView key="contentView" id="${makeId("clip")}"><rect key="frame" x="1" y="1" width="${Math.max(controlFrame.width - 18, 12)}" height="${Math.max(controlFrame.height - 2, 12)}"/><autoresizingMask key="autoresizingMask"/><subviews><textField identifier="${xml(control.identifier)}_PLACEHOLDER" fixedFrame="YES" translatesAutoresizingMaskIntoConstraints="NO" id="${makeId("placeholder")}"><rect key="frame" x="8" y="8" width="${Math.max(controlFrame.width - 40, 12)}" height="18"/><textFieldCell key="cell" title="${xml(control.title || control.identifier.replaceAll("_", " "))}" id="${makeId("cell")}"><font key="font" metaFont="smallSystem"/><color key="textColor" name="secondaryLabelColor" catalog="System" colorSpace="catalog"/><color key="backgroundColor" name="textBackgroundColor" catalog="System" colorSpace="catalog"/></textFieldCell></textField></subviews></clipView><scroller key="verticalScroller" verticalHuggingPriority="750" horizontal="NO" id="${makeId("scroll")}"><rect key="frame" x="${Math.max(controlFrame.width - 16, 0)}" y="1" width="15" height="${Math.max(controlFrame.height - 2, 12)}"/></scroller></scrollView>`;
   }

   if (control.style.includes("BS_AUTOCHECKBOX")) {
      return `<button ${attributes}>${rect(controlFrame)}${autoresizing}<buttonCell key="cell" type="check" title="${xml(control.title)}" bezelStyle="regularSquare" imagePosition="left" alignment="left" inset="2" id="${cellId}"><behavior key="behavior" changeContents="YES" doesNotDimImage="YES" lightByContents="YES"/><font key="font" metaFont="system"/></buttonCell></button>`;
   }

   if (control.style.includes("msctls_updown32")) {
      return `<stepper ${attributes} horizontalHuggingPriority="750" verticalHuggingPriority="750"><rect key="frame" x="${controlFrame.x}" y="${controlFrame.y}" width="19" height="27"/>${autoresizing}<stepperCell key="cell" continuous="YES" alignment="left" minValue="0" maxValue="65535" doubleValue="0" id="${cellId}"/></stepper>`;
   }

   if (control.style.includes("SS_BITMAP")) {
      return `<imageView ${attributes} horizontalHuggingPriority="251" verticalHuggingPriority="251" imageScaling="proportionallyUpOrDown">${rect(controlFrame)}${autoresizing}<imageCell key="cell" refusesFirstResponder="YES" alignment="left" imageScaling="proportionallyDown" id="${cellId}"/></imageView>`;
   }

   return `<box ${attributes} title="${xml(control.title || control.identifier.replaceAll("_", " "))}" boxType="custom" borderType="line">${rect(controlFrame)}${autoresizing}<view key="contentView" id="${makeId("box")}"><rect key="frame" x="1" y="1" width="${Math.max(controlFrame.width - 2, 12)}" height="${Math.max(controlFrame.height - 2, 12)}"/><autoresizingMask key="autoresizingMask"/></view></box>`;
}

function sceneXml(dialog, index) {
   const width = Math.max(Math.round(dialog.width * SX), MIN_WIDTH);
   const height = Math.max(Math.round(dialog.height * SY), MIN_HEIGHT);
   const windowController = makeId("wc");
   const window = makeId("window");
   const windowContentView = makeId("window-view");
   const viewController = makeId("vc");
   const contentView = makeId("view");
   const windowFirstResponder = makeId("responder");
   const viewFirstResponder = makeId("responder");
   const windowScene = makeId("scene");
   const viewScene = makeId("scene");
   const controls = dialog.controls.map((control) => controlXml(control, height, dialog.id)).join("\n                            ");

   return `        <!--${xml(dialog.id)}-->
        <scene sceneID="${windowScene}">
            <objects>
                <windowController storyboardIdentifier="${xml(dialog.id)}" id="${windowController}" userLabel="${xml(dialog.id)}" sceneMemberID="viewController">
                    <window key="window" title="${xml(dialog.title)}" allowsToolTipsWhenApplicationIsInactive="NO" autorecalculatesKeyViewLoop="NO" releasedWhenClosed="NO" visibleAtLaunch="NO" animationBehavior="default" id="${window}">
                        <windowStyleMask key="styleMask" titled="YES" closable="YES" miniaturizable="YES"/>
                        <rect key="contentRect" x="200" y="300" width="${width}" height="${height}"/>
                        <rect key="screenRect" x="0.0" y="0.0" width="1920" height="1080"/>
                        <view key="contentView" id="${windowContentView}">
                            <rect key="frame" x="0.0" y="0.0" width="${width}" height="${height}"/>
                            <autoresizingMask key="autoresizingMask"/>
                        </view>
                        <connections>
                            <outlet property="delegate" destination="${windowController}" id="${makeId("outlet")}"/>
                        </connections>
                    </window>
                    <connections>
                        <segue destination="${viewController}" kind="relationship" relationship="window.shadowedContentViewController" id="${makeId("segue")}"/>
                    </connections>
                </windowController>
                <customObject id="${windowFirstResponder}" userLabel="First Responder" customClass="NSResponder" sceneMemberID="firstResponder"/>
            </objects>
            <point key="canvasLocation" x="${(index % 4) * 700}" y="${Math.floor(index / 4) * 650}"/>
        </scene>
        <!--${xml(dialog.id)} Content-->
        <scene sceneID="${viewScene}">
            <objects>
                <viewController storyboardIdentifier="${xml(dialog.id)}_CONTENT" id="${viewController}" userLabel="${xml(dialog.id)} Content" sceneMemberID="viewController">
                    <view key="view" identifier="${xml(dialog.id)}_CONTENT_VIEW" id="${contentView}" userLabel="${xml(dialog.id)}">
                        <rect key="frame" x="0.0" y="0.0" width="${width}" height="${height}"/>
                        <autoresizingMask key="autoresizingMask"/>
                        <subviews>
                            ${controls}
                        </subviews>
                    </view>
                </viewController>
                <customObject id="${viewFirstResponder}" userLabel="First Responder" customClass="NSResponder" sceneMemberID="firstResponder"/>
            </objects>
            <point key="canvasLocation" x="${(index % 4) * 700 + 350}" y="${Math.floor(index / 4) * 650}"/>
        </scene>`;
}

const dialogs = parseDialogs(rc);
const scenes = dialogs.map(sceneXml).join("\n");
const output = `<?xml version="1.0" encoding="UTF-8"?>
<document type="com.apple.InterfaceBuilder3.Cocoa.Storyboard.XIB" version="3.0" toolsVersion="24765" targetRuntime="MacOSX.Cocoa" propertyAccessControl="none" useAutolayout="YES">
    <dependencies>
        <deployment identifier="macosx"/>
        <plugIn identifier="com.apple.InterfaceBuilder.CocoaPlugin" version="24765"/>
        <capability name="documents saved in the Xcode 8 format" minToolsVersion="8.0"/>
    </dependencies>
    <scenes>
${scenes}
    </scenes>
</document>
`;

fs.writeFileSync(storyboardPath, output);

const resourceNames = [...declaredResourceIds]
   .filter(([name, id]) => id >= 0 && /^(IDD_|IDC_)/.test(name))
   .map(([name]) => `setResourceName(${name}, "${name}");`)
   .join("\n");

fs.writeFileSync(resourceNamesPath, `${resourceNames}\n`);
console.log(`Converted ${dialogs.length} dialogs and wrote ${resourceNamesPath}`);
