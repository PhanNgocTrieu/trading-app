#pragma once

#include "infrastructure/db/sqlite_connection.hpp"

void runMigrations(SqliteConnection& db);
