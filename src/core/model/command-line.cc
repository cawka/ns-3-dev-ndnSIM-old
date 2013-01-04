/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#include <cstdlib> // for exit

#include "command-line.h"
#include "log.h"
#include "config.h"
#include "global-value.h"
#include "type-id.h"
#include "string.h"


NS_LOG_COMPONENT_DEFINE ("CommandLine");

namespace ns3 {

CommandLine::CommandLine ()
{
  NS_LOG_FUNCTION (this);
}
CommandLine::CommandLine (const CommandLine &cmd)
{
  Copy (cmd);
}
CommandLine &
CommandLine::operator = (const CommandLine &cmd)
{
  Clear ();
  Copy (cmd);
  return *this;
}
CommandLine::~CommandLine ()
{
  NS_LOG_FUNCTION (this);
  Clear ();
}
void
CommandLine::Copy (const CommandLine &cmd)
{
  NS_LOG_FUNCTION (&cmd);

  for (Items::const_iterator i = cmd.m_items.begin (); 
       i != cmd.m_items.end (); ++i)
    {
      m_items.push_back (*i);
    }
}
void
CommandLine::Clear (void)
{
  NS_LOG_FUNCTION (this);

  for (Items::const_iterator i = m_items.begin (); i != m_items.end (); ++i)
    {
      delete *i;
    }
  m_items.clear ();
}

CommandLine::Item::~Item ()
{
  NS_LOG_FUNCTION (this);
}

void
CommandLine::Parse (int iargc, char *argv[]) const
{
  NS_LOG_FUNCTION (this << iargc << argv);

  int argc = iargc;
  for (argc--, argv++; argc > 0; argc--, argv++)
    {
      // remove "--" or "-" heading.
      std::string param = *argv;
      std::string::size_type cur = param.find ("--");
      if (cur == 0)
        {
          param = param.substr (2, param.size () - 2);
        }
      else
        {
          cur = param.find ("-");
          if (cur == 0)
            {
              param = param.substr (1, param.size () - 1);
            }
          else
            {
              // invalid argument. ignore.
              continue;
            }
        }
      cur = param.find ("=");
      std::string name, value;
      if (cur == std::string::npos)
        {
          name = param;
          value = "";
        }
      else
        {
          name = param.substr (0, cur);
          value = param.substr (cur + 1, param.size () - (cur+1));
        }
      HandleArgument (name, value);
    }
}

void
CommandLine::PrintHelp (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);

  os << "--PrintHelp: Print this help message." << std::endl;
  os << "--PrintGroups: Print the list of groups." << std::endl;
  os << "--PrintTypeIds: Print all TypeIds." << std::endl;
  os << "--PrintGroup=[group]: Print all TypeIds of group." << std::endl;
  os << "--PrintAttributes=[typeid]: Print all attributes of typeid." << std::endl;
  os << "--PrintGlobals: Print the list of globals." << std::endl;
  if (!m_items.empty ())
    {
      os << "User Arguments:" << std::endl;
      for (Items::const_iterator i = m_items.begin (); i != m_items.end (); ++i)
        {
          os << "    --" << (*i)->m_name << ": " << (*i)->m_help << std::endl;
        }
    }
}

void
CommandLine::PrintGlobals (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);

  for (GlobalValue::Iterator i = GlobalValue::Begin (); i != GlobalValue::End (); ++i)
    {
      os << "    --" << (*i)->GetName () << "=[";
      Ptr<const AttributeChecker> checker = (*i)->GetChecker ();
      StringValue v;
      (*i)->GetValue (v);
      os << v.Get () << "]:  "
                << (*i)->GetHelp () << std::endl;
    }
}

void
CommandLine::PrintAttributes (std::ostream &os, std::string type) const
{
  NS_LOG_FUNCTION (this);

  TypeId tid;
  if (!TypeId::LookupByNameFailSafe (type, &tid))
    {
      NS_FATAL_ERROR ("Unknown type="<<type<<" in --PrintAttributes");
    }
  for (uint32_t i = 0; i < tid.GetAttributeN (); ++i)
    {
      os << "    --"<<tid.GetAttributeFullName (i)<<"=[";
      struct TypeId::AttributeInformation info = tid.GetAttribute (i);
      os << info.initialValue->SerializeToString (info.checker) << "]:  "
                << info.help << std::endl;
    }
}


void
CommandLine::PrintGroup (std::ostream &os, std::string group) const
{
  NS_LOG_FUNCTION (this);

  for (uint32_t i = 0; i < TypeId::GetRegisteredN (); ++i)
    {
      TypeId tid = TypeId::GetRegistered (i);
      if (tid.GetGroupName () == group)
        {
          os << "    --PrintAttributes=" <<tid.GetName ()<<std::endl;
        }
    }
}

void
CommandLine::PrintTypeIds (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);

  for (uint32_t i = 0; i < TypeId::GetRegisteredN (); ++i)
    {
      TypeId tid = TypeId::GetRegistered (i);
      os << "    --PrintAttributes=" <<tid.GetName ()<<std::endl;
    }
}

void
CommandLine::PrintGroups (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);

  std::list<std::string> groups;
  for (uint32_t i = 0; i < TypeId::GetRegisteredN (); ++i)
    {
      TypeId tid = TypeId::GetRegistered (i);
      std::string group = tid.GetGroupName ();
      if (group == "")
        {
          continue;
        }
      bool found = false;
      for (std::list<std::string>::const_iterator j = groups.begin (); j != groups.end (); ++j)
        {
          if (*j == group)
            {
              found = true;
              break;
            }
        }
      if (!found)
        {
          groups.push_back (group);
        }
    }
  for (std::list<std::string>::const_iterator k = groups.begin (); k != groups.end (); ++k)
    {
      os << "    --PrintGroup="<<*k<<std::endl;
    }
}

void
CommandLine::HandleArgument (std::string name, std::string value) const
{
  NS_LOG_FUNCTION (this << name << value);

  NS_LOG_DEBUG ("Handle arg name="<<name<<" value="<<value);
  if (name == "PrintHelp")
    {
      // method below never returns.
      PrintHelp (std::cout);
      std::exit (0);
    } 
  else if (name == "PrintGroups")
    {
      // method below never returns.
      PrintGroups (std::cout);
      std::exit (0);
    }
  else if (name == "PrintTypeIds")
    {
      // method below never returns.
      PrintTypeIds (std::cout);
      std::exit (0);
    }
  else if (name == "PrintGlobals")
    {
      // method below never returns.
      PrintGlobals (std::cout);
      std::exit (0);
    }
  else if (name == "PrintGroup")
    {
      // method below never returns.
      PrintGroup (std::cout, value);
      std::exit (0);
    }
  else if (name == "PrintAttributes")
    {
      // method below never returns.
      PrintAttributes (std::cout, value);
      std::exit (0);
    }
  else
    {
      for (Items::const_iterator i = m_items.begin (); i != m_items.end (); ++i)
        {
          if ((*i)->m_name == name)
            {
              if (!(*i)->Parse (value))
                {
                  std::cerr << "Invalid argument value: "<<name<<"="<<value << std::endl;
                  std::exit (1);
                }
              else
                {
                  return;
                }
            }
        }
    }
  if (!Config::SetGlobalFailSafe (name, StringValue (value))
      && !Config::SetDefaultFailSafe (name, StringValue (value)))
    {
      std::cerr << "Invalid command-line arguments: --"<<name<<"="<<value<<std::endl;
      PrintHelp (std::cerr);
      std::exit (1);
    }
}

bool
CommandLine::CallbackItem::Parse (std::string value)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("CommandLine::CallbackItem::Parse \"" << value << "\"");
  return m_callback (value);
}

void
CommandLine::AddValue (const std::string &name,
                       const std::string &help,
                       Callback<bool, std::string> callback)
{
  NS_LOG_FUNCTION (this << &name << &help << &callback);
  CallbackItem *item = new CallbackItem ();
  item->m_name = name;
  item->m_help = help;
  item->m_callback = callback;
  m_items.push_back (item);
}

} // namespace ns3
