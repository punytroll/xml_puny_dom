/**
 * galactic-fall
 * Copyright (C) 2022  Hagen Möbius
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

#include <cassert>
#include <memory>
#include <sstream>

#include <xml_puny_dom/xml_puny_dom.h>

auto ReadDOMFromString(std::string const & String) -> std::unique_ptr<XML::Document>
{
    auto StringStream = std::istringstream{String};
    
    return std::make_unique<XML::Document>(StringStream);
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] char * argv[]) -> int
{
    {
        auto DOM = ReadDOMFromString("<root/>");
        
        assert(DOM->GetDocumentElement() != nullptr);
        assert(DOM->GetDocumentElement()->GetName() == "root");
        assert(DOM->GetDocumentElement()->GetChildNodes().size() == 0);
    }
    {
        auto DOM = ReadDOMFromString("<root> </root>");
        
        assert(DOM->GetDocumentElement() != nullptr);
        assert(DOM->GetDocumentElement()->GetName() == "root");
        assert(DOM->GetDocumentElement()->GetChildNodes().size() == 1);
        assert(DOM->GetDocumentElement()->GetChildElements().size() == 0);
    }
    {
        auto DOM = ReadDOMFromString("<root><a/><b/><c/></root>");
        
        assert(DOM->GetDocumentElement() != nullptr);
        assert(DOM->GetDocumentElement()->GetName() == "root");
        assert(DOM->GetDocumentElement()->GetChildNodes().size() == 3);
        assert(DOM->GetDocumentElement()->GetChildElements().size() == 3);
    }
}
