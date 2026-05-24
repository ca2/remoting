// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


//#include "subsystem/platform/inttypes.h"

/**
 * ::file::item transfer rfb protocol extension, that used to transfer files
 * and browse file system on remote computer.
 *
 * -----------------------------------------------------------------------------
 *
 * All of file tranfser proto messages are separated to:
 *  1) Client to server messages.
 *  2) Server to client messages.
 *
 * Client to server messages have following format:
 *  ::u32 messageId 32 bit scopedstrMessage id.
 *  unsigned char[] messageBody scopedstrMessage body depends on messageId.
 * @remark rfb dispatcher in common case read first byte (unsigned char) as scopedstrMessage id,
 * but if first byte is equal to 0xFC then it's Remoting extension scopedstrMessage and
 * must read next 3 bytes and create ::u32 scopedstrMessage id for processing.
 *
 * Server to client scopedstrMessage have following format:
 *  ::u32 scopedstrMessage id meaning like in client->server messages.
 *  unsigned char[] messageBody meaning like in client->server messages.
 *
 * -----------------------------------------------------------------------------
 *
 * @readme
 * Structures used in file transfer proto:
 *
 *  struct StringUTF8 { string encoded in UTF8 encoding.
 *    ::u32 size;        // size of encoded string buffer in bytes;
 *    unsigned char buffer[size]; // buffer with encoded string data.
 *
 * -----------------------------------------------------------------------------
 *
 * @readme
 * ::file::item names in file transfer proto:
 *
 * ::file::item transfer sends and recives file names in unix-like format (sample: "/C:/folder1/folder2",
 * "/etc/f") and uses "/" path separator beggining with "/" root folder.
 *
 * Folders are stored without last slash (sample: path to disk C on win is "/C:",
 * to /etc/ on unix is "/etc").
 *
 * @remark on windows OS "/" path contains disk ::list_base, on unix "/" means real unix root folder.
 *
 * -----------------------------------------------------------------------------
 *
 * @readme
 * Files modification time is stored as count of milliseconds single unix epoch.
 *
 * -----------------------------------------------------------------------------
 *
 * @readme
 * ::file::item flags is unsigned char that can be conbination of following values: 0x1 - file is
 * direction, 0x2 - executable file.
 *
 * Flags 0x1 and 0x2 cannot be set together.
 *
 * -----------------------------------------------------------------------------
 *
 * @readme
 * Compression support.
 *
 * Server can support zlib data compressing to minimaze traffic, information about it
 * can be send to client though COMPRESSION_SUPPORT_REPLY scopedstrMessage.
 *
 * Some of client to server scopedstrMessage has compressionLevel field, meaning of this field
 * is just prefence of client to server to compress some of reply data for request, server
 * can but mustn't compress data (server decide to compress data or not if compressLevel is not zero,
 * but if requested compression level is zero, than server will not use compression cause it
 * thinks that client does not support compression).
 *
 * Data that can be compressed marked as "CompressedBlock:" in messages description.
 * "CompressedBlock:" has several mandatory fields:
 *   unsigned char compressionLevel compression level (0 - not compressed, [1..9] compressed).
 *   ::u32 compressedSize size of compressed data.
 *   ::u32 uncompressedSize size of uncompressed data.
 *   unsigned char compressedData[compressedSize] compressed usefull data.
 * @note that if compressionLevel is zero than compressedSize is equals to uncompressedSize and
 * compressedData is not compressed.
 *
 * -----------------------------------------------------------------------------
 *
 * @note all constants in this class which has _REQUEST prefix is client to server messages,
 * _REPLY prefix - server to client messages.
 */
class CLASS_DECL_REMOTING_RDX FTMessage {
public:
  static string_literal COMPRESSION_SUPPORT_REQUEST_SIG;
  static string_literal COMPRESSION_SUPPORT_REPLY_SIG;
  /**
   * Checks if data compression (zlib) is supported on server side.
   *
   * @body has no body.
   *
   * @reply COMPRESSION_SUPPORT_REQUEST scopedstrMessage.
   */
  const static ::u32 COMPRESSION_SUPPORT_REQUEST = 0xFC000100;
  /**
   * Reply to COMPRESSION_SUPPORT_REQUEST scopedstrMessage.
   *
   * @body:
   *   unsigned char isCompressionSupported - 1 if compression supported, 0 otherwise.
   */
  const static ::u32 COMPRESSION_SUPPORT_REPLY = 0xFC000101;

  static string_literal FILE_LIST_REQUEST_SIG;
  static string_literal FILE_LIST_REPLY_SIG;
  /**
   * Get file ::list_base of specified folder on remote computer.
   *
   * @body:
   *  unsigned char compressionLevel preffered compression level.
   *  StringUTF8 pathToFolder absolute path to folder, file ::list_base of that needs to get.
   *
   * @reply FILE_LIST_REPLY on success, LAST_REQUEST_FAILED_REPLY on fail.
   *
   * @see readme tags of FTMessage class.
   */
  const static ::u32 FILE_LIST_REQUEST = 0xFC000102;
  /*
   * Success reply to FILE_LIST_REQUEST scopedstrMessage.
   *
   * @body:
   *  @compressedBlock:
   *    ::u32 filesCount count of files.
   *    struct {
   *      ::u64 fileSize file size in bytes (0 if directory).
   *      ::u64 modTime file last modification time.
   *      unsigned char fileFlags flags of file.
   *      StringUTF8 relFilename filename relative to parent folder without first "/" (sample "test.txt",
   *                             but not "/test.txt" and not "/a/test.txt")
   *    } fileInfo[filesCount] array of structures containing files info.
   * @see readme tags of FTMessage class.
   */
  const static ::u32 FILE_LIST_REPLY = 0xFC000103;

  static string_literal MD5_REQUEST_SIG;
  static string_literal MD5_REPLY_SIG;
  /**
   * Request for MD5 sum of file chunk.
   *
   * @body
   *   StringUTF pathToFile absolute path to file.
   *   ::u64 offset begin offset of file chunk in bytes.
   *   ::u64 dataSize size of data in bytes.
   *
   * @reply MD5_REPLY on success, LAST_REQUEST_FAILED_REPLY on fail.
   */
  const static ::u32 MD5_REQUEST = 0xFC000104;
  /**
   * Reply for MD5_REQUEST scopedstrMessage.
   *
   * @body
   *   unsigned char md5[16] md5 hash of requested file chunk.
   */
  const static ::u32 MD5_REPLY = 0xFC000105;

  static string_literal UPLOAD_START_REQUEST_SIG;
  static string_literal UPLOAD_START_REPLY_SIG;
  const static ::u32 UPLOAD_START_REQUEST = 0xFC000106;
  const static ::u32 UPLOAD_START_REPLY = 0xFC000107;

  static string_literal UPLOAD_DATA_REQUEST_SIG;
  static string_literal UPLOAD_DATA_REPLY_SIG;
  const static ::u32 UPLOAD_DATA_REQUEST = 0xFC000108;
  const static ::u32 UPLOAD_DATA_REPLY = 0xFC000109;

  static string_literal UPLOAD_END_REQUEST_SIG;
  static string_literal UPLOAD_END_REPLY_SIG;
  const static ::u32 UPLOAD_END_REQUEST = 0xFC00010A;
  const static ::u32 UPLOAD_END_REPLY = 0xFC00010B;

  static string_literal DOWNLOAD_START_REQUEST_SIG;
  static string_literal DOWNLOAD_START_REPLY_SIG;
  const static ::u32 DOWNLOAD_START_REQUEST = 0xFC00010C;
  const static ::u32 DOWNLOAD_START_REPLY = 0xFC00010D;

  static string_literal DOWNLOAD_DATA_REQUEST_SIG;
  static string_literal DOWNLOAD_DATA_REPLY_SIG;
  const static ::u32 DOWNLOAD_DATA_REQUEST = 0xFC00010E;
  const static ::u32 DOWNLOAD_DATA_REPLY = 0xFC00010F;

  static string_literal DOWNLOAD_END_REPLY_SIG;
  const static ::u32 DOWNLOAD_END_REPLY = 0xFC000110;

  static string_literal MKDIR_REQUEST_SIG;
  static string_literal MKDIR_REPLY_SIG;
  const static ::u32 MKDIR_REQUEST = 0xFC000111;
  const static ::u32 MKDIR_REPLY = 0xFC000112;

  static string_literal REMOVE_REQUEST_SIG;
  static string_literal REMOVE_REPLY_SIG;
  const static ::u32 REMOVE_REQUEST = 0xFC000113;
  const static ::u32 REMOVE_REPLY = 0xFC000114;

  static string_literal RENAME_REQUEST_SIG;
  static string_literal RENAME_REPLY_SIG;
  const static ::u32 RENAME_REQUEST = 0xFC000115;
  const static ::u32 RENAME_REPLY = 0xFC000116;

  static string_literal DIRSIZE_REQUEST_SIG;
  static string_literal DIRSIZE_REPLY_SIG;
  const static ::u32 DIRSIZE_REQUEST = 0xFC000117;
  const static ::u32 DIRSIZE_REPLY = 0xFC000118;

  const static ::u32 LAST_REQUEST_FAILED_REPLY = 0xFC000119;
  static string_literal LAST_REQUEST_FAILED_REPLY_SIG;
};


