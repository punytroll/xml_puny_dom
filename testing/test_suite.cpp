/**
 * galactic-fall
 * Copyright (C) 2022  Hagen Möbius
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
**/

#include <cassert>
#include <memory>
#include <ranges>
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
        assert(XML::IsText(DOM->GetDocumentElement()->GetChildNodes()[0]) == true);
    }
    {
        auto DOM = ReadDOMFromString("<root><a/><b/><c/></root>");
        
        assert(DOM->GetDocumentElement() != nullptr);
        assert(DOM->GetDocumentElement()->GetName() == "root");
        assert(DOM->GetDocumentElement()->GetChildNodes().size() == 3);
        assert(XML::IsElement(DOM->GetDocumentElement()->GetChildNodes()[0]) == true);
        assert(XML::IsElement(DOM->GetDocumentElement()->GetChildNodes()[1]) == true);
        assert(XML::IsElement(DOM->GetDocumentElement()->GetChildNodes()[2]) == true);
    }
    {
        auto DOM = ReadDOMFromString("<root><a/>  <b/><c/>   </root>");
        
        assert(DOM->GetDocumentElement() != nullptr);
        assert(DOM->GetDocumentElement()->GetName() == "root");
        assert(DOM->GetDocumentElement()->GetChildNodes().size() == 5);
        assert(XML::IsElement(DOM->GetDocumentElement()->GetChildNodes()[0]) == true);
        assert(XML::IsText(DOM->GetDocumentElement()->GetChildNodes()[1]) == true);
        assert(XML::IsElement(DOM->GetDocumentElement()->GetChildNodes()[2]) == true);
        assert(XML::IsElement(DOM->GetDocumentElement()->GetChildNodes()[3]) == true);
        assert(XML::IsText(DOM->GetDocumentElement()->GetChildNodes()[4]) == true);
        assert(std::ranges::distance(DOM->GetDocumentElement()->GetChildNodes() | std::views::filter(XML::IsElement)) == 3);
        assert(std::ranges::distance(DOM->GetDocumentElement()->GetChildNodes() | std::views::filter(XML::IsText)) == 2);
    }
    {
        auto DOM = ReadDOMFromString("<root><a/>  <b/><c/>   </root>");
        auto Result = std::string{""};
        
        for(auto String : DOM->GetDocumentElement()->GetChildElements() | std::views::transform([](XML::Element const * Element) { return Element->GetName(); }))
        {
            Result += String;
        }
        assert(Result == "abc");
    }
    {
        auto DOM = ReadDOMFromString("<root><a>1</a>2<b/>3<b/>45<c>6<d>7</d><d/></c> 8</root>");
        auto Result = std::string{""};
        
        for(auto String : DOM->GetDocumentElement()->GetChildTexts() | std::views::transform([](XML::Text const * Text) { return Text->GetText(); }))
        {
            Result += String;
        }
        assert(Result == "2345 8");
    }
    
    return 0;
}
