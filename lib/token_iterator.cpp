// Metashell - Interactive C++ template metaprogramming shell
// Copyright (C) 2013, Abel Sinkovics (abel@sinkovics.hu)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <metashell/token_iterator.hpp>
#include <metashell/shell.hpp>

using namespace metashell;

token_iterator metashell::begin_tokens(const std::string& s_)
{
  return
    token_iterator(
      s_.begin(),
      s_.end(),
      token_iterator::value_type::position_type(shell::input_filename()),
      boost::wave::language_support(
        boost::wave::support_cpp
        | boost::wave::support_option_long_long
      )
    );
}

