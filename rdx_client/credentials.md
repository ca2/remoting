# Saved credentials

The Windows client manages its own credentials using Windows Credential Manager.
The default is enabled; no password belongs in a `.remoting_rdx` file.

```ini
host=your-host:55447
use_redirection_server_name=1
use_windows_credential_manager=1
```

On the first connection, enter your credentials in the modern Windows Security
dialog and select its save checkbox. The client explicitly writes a Generic
Credential using `CredWriteW` when you submit the dialog with save selected.
Saving does not depend on the server emitting the RDP login-complete event.
On subsequent connections it reads the same entry with `CredReadW` without
showing the dialog. Canceling the dialog creates no entry. An unchecked save
checkbox creates or updates no entry and leaves existing entries unchanged.

A saved password is not necessarily valid: if login fails or the password changes,
use `prompt_for_credentials=1` to replace it.

Entries are Generic Credentials named `remoting_rdx/<punycode-host>:<port>`.
Hostnames are lowercased, and an omitted port means 3389. Entries are separate
from the native RDP client's `TERMSRV` credentials and are not migrated automatically.
No existing credentials are deleted.

To replace a saved password, temporarily add:

```ini
prompt_for_credentials=1
```

Reconnect, enter the new password, select save, and finish logging in. Remove
that line afterward so saved credentials can be reused without a prompt. If
credentials are rejected, reconnect with this setting to replace them.

To use the original ActiveX-managed credential dialog instead:

```ini
use_windows_credential_manager=0
```

## Manual verification

1. Build and restart the client, then open a connection with no saved application entry.
2. Cancel the dialog: the connection must not start and no new entry should appear.
3. Reopen it, enter valid credentials, select save, and complete login.
4. Close and reopen the client: the initial credential prompt should be skipped.
5. Use `prompt_for_credentials=1` to replace the password, then verify reuse again.
6. Repeat with save unchecked: a connection without an existing entry should prompt again.
7. Confirm a different port uses a separate entry and Unicode/Punycode spellings use the same entry.

The application clears temporary password buffers after passing them to the RDP
control. Password entry and reconnect verification require the user; they are not
performed by the coding agent.

## GNOME Remote Login

The Windows prompt authenticates access to the GNOME remote login service.
The GDM login screen inside that connection separately authenticates the Linux
user. Saving the first credential does not provide single sign-on to GDM.
The client does not type or forward that password into the remote login screen.
