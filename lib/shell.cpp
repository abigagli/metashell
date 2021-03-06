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

#include "metashell.hpp"
#include "indenter.hpp"

#include <metashell/shell.hpp>
#include <metashell/version.hpp>
#include <metashell/token_iterator.hpp>

#include <boost/wave/cpplexer/cpplexer_exceptions.hpp>

#include <boost/foreach.hpp>

#include <cctype>

using namespace metashell;

namespace
{
  bool has_typedef(const token_iterator& begin_, const token_iterator& end_)
  {
    return std::find(begin_, end_, boost::wave::T_TYPEDEF) != end_;
  }

  bool is_environment_setup_command(const std::string& s_)
  {
    try
    {
      const token_iterator it = begin_tokens(s_), end;

      if (it == end)
      {
        // empty input is not a query
        return true;
      }
      else
      {
        const boost::wave::token_id id = *it;
        if (IS_CATEGORY(id, boost::wave::KeywordTokenType))
        {
          switch (id)
          {
          case boost::wave::T_BOOL:
          case boost::wave::T_CHAR:
          case boost::wave::T_CONST:
          case boost::wave::T_DOUBLE:
          case boost::wave::T_FLOAT:
          case boost::wave::T_INT:
          case boost::wave::T_LONG:
          case boost::wave::T_SHORT:
          case boost::wave::T_SIGNED:
          case boost::wave::T_UNSIGNED:
          case boost::wave::T_VOID:
          case boost::wave::T_VOLATILE:
          case boost::wave::T_WCHART:
            return has_typedef(it, end);
          case boost::wave::T_SIZEOF:
          case boost::wave::T_CONSTCAST:
          case boost::wave::T_STATICCAST:
          case boost::wave::T_DYNAMICCAST:
          case boost::wave::T_REINTERPRETCAST:
            return false;
          default:
            return true;
          }
        }
        else if (IS_CATEGORY(id, boost::wave::IdentifierTokenType))
        {
          return has_typedef(it, end);
        }
        else
        {
          return IS_CATEGORY(id, boost::wave::PPTokenType);
        }
      }
    }
    catch (...)
    {
      return false;
    }
  }

  void display(const result& r_, shell& s_)
  {
    if (!r_.info.empty())
    {
      s_.display_info(r_.info);
    }

    BOOST_FOREACH(const std::string& i, r_.errors)
    {
      s_.display_error(i);
    }
    if (!r_.has_errors())
    {
      s_.display_normal(r_.output);
    }
  }

  bool has_non_whitespace(const std::string& s_)
  {
    BOOST_FOREACH(char c, s_)
    {
      if (!std::isspace(c))
      {
        return true;
      }
    }
    return false;
  }

  bool is_empty_line(const std::string& s_)
  {
    try
    {
      token_iterator i = begin_tokens(s_), e;
      while (i != e)
      {
        try
        {
          if (
            !(
              IS_CATEGORY(*i, boost::wave::WhiteSpaceTokenType)
              || IS_CATEGORY(*i, boost::wave::EOFTokenType)
              || IS_CATEGORY(*i, boost::wave::EOLTokenType)
            )
          )
          {
            return false;
          }
          ++i;
        }
        catch (const boost::wave::cpplexer::lexing_exception& e)
        {
          if (!e.is_recoverable())
          {
            throw;
          }
        }
      }
    }
    catch (...)
    {
      return false;
    }
    return true;
  }
}

shell::shell(const config& config_) : _config(config_) {}

shell::~shell() {}

void shell::cancel_operation() {}

void shell::display_splash() const
{
  const std::string version_desc =
    #include "version_desc.hpp"
  ;

  indenter ind(width(), " * ");
  ind
    .raw("/*")
    .left_align("Template metaprogramming shell " + version());

  if (!version_desc.empty())
  {
    ind.left_align(version_desc);
  }

  display_normal(
    ind
      .empty_line()
      .left_align(
        "Metashell Copyright (C) 2013 Abel Sinkovics (abel@sinkovics.hu)"
      )
      .left_align(
        "This program comes with ABSOLUTELY NO WARRANTY. This is free software,"
        " and you are welcome to redistribute it under certain conditions;"
        " for details visit <http://www.gnu.org/licenses/>."
      )
      .empty_line()
      .left_align("Based on")
      .left_align(
        metashell::libclang_version(),
        " *              ",
        " *   libclang   "
      )
      .left_align(
        metashell::wave_version(),
        " *              ",
        " *   Boost.Wave "
      )
      .left_align(
        metashell::readline_version(),
        " *              ",
        " *   Readline   "
      )
      .raw(" */")
      .str()
  );
  if (_config.verbose)
  {
    display_normal(
      indenter(width(), "// ")
        .left_align("Verbose mode: ON")
        .str()
    );
  }
}

void shell::line_available(const std::string& s_)
{
  if (has_non_whitespace(s_))
  {
    if (_prev_line != s_)
    {
      add_history(s_);
      _prev_line = s_;
    }

    if (!is_empty_line(s_))
    {
      if (is_environment_setup_command(s_))
      {
        store_in_buffer(s_);
      }
      else
      {
        display(eval_tmp(_buffer, s_, _config), *this);
      }
    }
  }
}

std::string shell::prompt() const
{
  return "> ";
}

bool shell::store_in_buffer(const std::string& s_)
{
  const std::string new_buffer = append_to_buffer(_buffer, s_);
  const result r = validate_code(new_buffer, _config);
  const bool success = !r.has_errors();
  if (success)
  {
    _buffer = new_buffer;
  }
  display(r, *this);
  return success;
}

const char* shell::input_filename()
{
  return "<stdin>";
}

void shell::code_complete(
  const std::string& s_,
  std::set<std::string>& out_
) const
{
  metashell::code_complete(_buffer, s_, _config, out_);
}


