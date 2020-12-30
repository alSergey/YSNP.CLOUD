#pragma once

#include <string>

const std::string createQueryFiles = "CREATE TABLE IF NOT EXISTS  \"Files\" (\n"
                                     "\t\"id\"\tINTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,\n"
                                     "\t\"file_name\"\tTEXT NOT NULL,\n"
                                     "\t\"file_extention\"\tTEXT NOT NULL,\n"
                                     "\t\"file_size\"\tINTEGER NOT NULL,\n"
                                     "\t\"file_path\"\tTEXT NOT NULL,\n"
                                     "\t\"count_chunks\"\tINTEGER NOT NULL,\n"
                                     "\t\"version\"\tINTEGER NOT NULL,\n"
                                     "\t\"is_download\"\tBOOLEAN NOT NULL,\n"
                                     "\t\"update_date\"\tTEXT NOT NULL,\n"
                                     "\t\"create_date\"\tTEXT NOT NULL\n"
                                     ");";

const std::string createQueryChunks = "CREATE TABLE IF NOT EXISTS \"Chunks\" (\n"
                                      "\t\"id\"\tINTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,\n"
                                      "\t\"id_file\"\tINTEGER NOT NULL,\n"
                                      "\t\"chunk_order\"\tINTEGER NOT NULL,\n"
                                      "\t\"chunk_size\"\tINTEGER,\n"
                                      "\t\"rapid_hash\"\tTEXT,\n"
                                      "\t\"static_hash\"\tTEXT\n"
                                      ");";

const std::string createQueryUser = "CREATE TABLE IF NOT EXISTS \"User\" (\n"
                                    "\t\"id\"\tINTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,\n"
                                    "\t\"user_id\"\tINTEGER NOT NULL,\n"
                                    "\t\"login\"\tTEXT NOT NULL,\n"
                                    "\t\"password\"\tTEXT NOT NULL,\n"
                                    "\t\"device_id\"\tINTEGER NOT NULL,\n"
                                    "\t\"device_name\"\tINTEGER NOT NULL,\n"
                                    "\t\"sync_folder\"\tTEXT NOT NULL,\n"
                                    "\t\"last_update\"\tTEXT NOT NULL\n"
                                    ");";
