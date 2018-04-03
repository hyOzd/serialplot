/*
  Copyright © 2018 Hasan Yavuz Özderya

  This file is part of serialplot.

  serialplot is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  serialplot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
*/

// This tells Catch to provide a main() - only do this in one cpp file per executable
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <QBuffer>
#include "binarystreamreader.h"

TEST_CASE("creating a BinaryStreamReader", "[reader]")
{
    QBuffer buffer;

    BinaryStreamReader bs(&buffer);
}

// Note: this is added because `QApplication` is a must be created for widgets
#include <QApplication>
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return result;
}
