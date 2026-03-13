/**
 * galactic-fall
 * Copyright (C) 2006-2022  Hagen Möbius
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

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stack>
#include <stdexcept>

#include <xml_parser/parser.h>

#include <xml_puny_dom/xml_puny_dom.h>

namespace XML
{
    class DOMReader : public XML::Parser
    {
    public:
        DOMReader(std::istream & Stream, XML::Document * Document, XML::Element ** RootElement);
        auto ElementStart(std::string const & Name, std::map<std::string, std::string> const & Attributes, XML::Location const & StartLocation) -> void override;
        auto ElementEnd(std::string const & Name) -> void override;
        auto Text(std::string const & Text, XML::Location const & StartLocation) -> void override;
    private:
        XML::Document * m_Document;
        XML::Element ** m_RootElement;
        std::stack<XML::Element *> m_ElementStack;
    };
}

XML::DOMReader::DOMReader(std::istream & Stream, XML::Document * Document, XML::Element ** RootElement) :
    XML::Parser{Stream},
    m_Document{Document},
    m_RootElement{RootElement}
{
}

void XML::DOMReader::ElementStart(std::string const & Name, std::map<std::string, std::string> const & Attributes, XML::Location const & StartLocation)
{
    if(*m_RootElement == nullptr)
    {
        // we've got the root element
        m_ElementStack.push(*m_RootElement = new XML::Element{m_Document, StartLocation.Line, StartLocation.Column, Name, Attributes});
    }
    else
    {
        m_ElementStack.push(new XML::Element{m_ElementStack.top(), StartLocation.Line, StartLocation.Column, Name, Attributes});
    }
}

void XML::DOMReader::ElementEnd(std::string const & Name)
{
    if(m_ElementStack.size() == 0)
    {
        std::cerr << "Got '/" << Name << "' but stack is empty." << std::endl;
        
        throw std::domain_error{Name};
    }
    if(Name != m_ElementStack.top()->GetName())
    {
        std::cerr << "Got '/" << Name << "' but expected '/" << m_ElementStack.top()->GetName() << "'." << std::endl;
        
        throw std::domain_error{Name};
    }
    m_ElementStack.pop();
}

void XML::DOMReader::Text(std::string const & Text, XML::Location const & StartLocation)
{
    if(m_ElementStack.size() > 0)
    {
        new XML::Text{m_ElementStack.top(), StartLocation.Line, StartLocation.Column, Text};
    }
    else
    {
        std::cerr << "Misformed XML: Got text \"" << Text << "\" outside of root element." << std::endl;
        
        throw std::domain_error("NoElement");
    }
}

XML::Node::Node(XML::NodeType NodeType, XML::Node * ParentNode, std::uint64_t SourceLine, std::uint64_t SourceColumn) :
    m_NodeType{NodeType},
    m_ParentNode{ParentNode},
    m_SourceColumn{SourceColumn},
    m_SourceLine{SourceLine}
{
    if(m_ParentNode != nullptr)
    {
        m_ParentNode->m_ChildNodes.push_back(this);
    }
}

XML::Node::~Node()
{
    // delete it from the parent's child nodes vector
    if(m_ParentNode != nullptr)
    {
        auto Iterator = std::find(m_ParentNode->m_ChildNodes.begin(), m_ParentNode->m_ChildNodes.end(), this);
        
        assert(Iterator != m_ParentNode->m_ChildNodes.end());
        m_ParentNode->m_ChildNodes.erase(Iterator);
        m_ParentNode = nullptr;
    }
    while(m_ChildNodes.empty() == false)
    {
        // erasing the child from the m_ChildNodes vector will happen in the childs destructor
        delete m_ChildNodes.front();
    }
}

auto XML::Node::GetChildNodes() const -> std::vector<XML::Node *> const &
{
    return m_ChildNodes;
}

auto XML::Node::GetChildNode(std::vector< XML::Node * >::size_type Index) const -> XML::Node const *
{
    return m_ChildNodes[Index];
}

auto XML::Node::GetNodeType() const -> XML::NodeType
{
    return m_NodeType;
}

auto XML::Node::GetParentNode() const -> XML::Node const *
{
    return m_ParentNode;
}

auto XML::Node::GetSourceColumn() const -> std::uint64_t
{
    return m_SourceColumn;
}

auto XML::Node::GetSourceLine() const -> std::uint64_t
{
    return m_SourceLine;
}

XML::Element::Element(XML::Node * ParentNode, std::uint64_t SourceLine, std::uint64_t SourceColumn, std::string const & Name, std::map<std::string, std::string> const & Attributes) :
    XML::Node{XML::NodeType::Element, ParentNode, SourceLine, SourceColumn},
    m_Name{Name},
    m_Attributes{Attributes}
{
}

auto XML::Element::GetName() const -> std::string const &
{
    return m_Name;
}

auto XML::Element::GetAttributes() const -> std::map<std::string, std::string> const &
{
    return m_Attributes;
}

auto XML::Element::GetAttribute(std::string const & AttributeName) const -> std::string const &
{
    return m_Attributes.find(AttributeName)->second;
}

auto XML::Element::HasAttribute(std::string const & AttributeName) const -> bool
{
    return m_Attributes.find(AttributeName) != m_Attributes.end();
}

XML::Text::Text(XML::Node * ParentNode, std::uint64_t SourceLine, std::uint64_t SourceColumn, std::string const & Text) :
    XML::Node{XML::NodeType::Text, ParentNode, SourceLine, SourceColumn},
    m_Text{Text}
{
}

auto XML::Text::GetText() const -> std::string const &
{
    return m_Text;
}

XML::Document::Document(std::istream & Stream) :
    XML::Node{XML::NodeType::Document, nullptr, 0, 0},
    m_DocumentElement{nullptr}
{
    auto DOMReader = XML::DOMReader{Stream, this, &m_DocumentElement};
    
    DOMReader.Parse();
}

XML::Document::~Document()
{
    delete m_DocumentElement;
    m_DocumentElement = nullptr;
}

auto XML::Document::GetDocumentElement() const -> XML::Element const *
{
    return m_DocumentElement;
}

auto XML::IsElement(XML::Node const * Node) -> bool
{
    return Node->GetNodeType() == XML::NodeType::Element;
}

auto XML::IsText(XML::Node const * Node) -> bool
{
    return Node->GetNodeType() == XML::NodeType::Text;
}
