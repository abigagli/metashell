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

#include "test_shell.hpp"

using namespace metashell;

test_shell::test_shell() :
  shell(config::default_config),
  _width(80),
  _history(0)
{}

test_shell::test_shell(const config& cfg_, int width_) :
  shell(cfg_),
  _width(width_),
  _history(0)
{}

test_shell::test_shell(std::vector<std::string>& history_) :
  shell(config::default_config),
  _width(80),
  _history(&history_)
{}

void test_shell::display_normal(const std::string& s_) const
{
  _output += s_;
}

void test_shell::display_info(const std::string& s_) const
{
  _info += s_;
}

void test_shell::display_error(const std::string& s_) const
{
  _error += s_;
}

const std::string& test_shell::output() const
{
  return _output;
}

const std::string& test_shell::info() const
{
  return _info;
}

const std::string& test_shell::error() const
{
  return _error;
}

unsigned int test_shell::width() const
{
  return _width;
}

void test_shell::add_history(const std::string& s_)
{
  if (_history)
  {
    _history->push_back(s_);
  }
}

