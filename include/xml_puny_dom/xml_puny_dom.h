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

#ifndef XML_PUNY_DOM__XML_PUNY_DOM_H
#define XML_PUNY_DOM__XML_PUNY_DOM_H

#include <istream>
#include <vector>
#include <map>
#include <string>

namespace XML
{
    enum class NodeType
    {
        Element,
        Document,
        Text
    };
    
    class Document;
    class Element;
    
    class Node
    {
        friend class XML::Element;
    public:
        Node(XML::NodeType NodeType, XML::Node * ParentNode);
        virtual ~Node(void);
        auto GetChildNodes(void) const -> std::vector<XML::Node *> const &;
        auto GetChildNode(std::vector<XML::Node *>::size_type Index) const -> XML::Node const *;
        auto GetNodeType(void) const -> XML::NodeType;
        auto GetParentNode(void) const -> XML::Node const *;
    protected:
        std::vector<XML::Node *> m_ChildNodes;
        XML::NodeType m_NodeType;
        XML::Node * m_ParentNode;
    };
    
    class Element : public XML::Node
    {
    public:
        Element(XML::Node * Parent, std::string const & Name, std::map<std::string, std::string> const & Attributes);
        auto GetName(void) const -> std::string const &;
        auto GetAttributes(void) const -> std::map<std::string, std::string> const &;
        auto GetAttribute(std::string const & AttributeName) const -> std::string const &;
        auto HasAttribute(std::string const & AttributeName) const -> bool;
    private:
        std::string m_Name;
        std::map<std::string, std::string> m_Attributes;
    };
    
    class Text : public XML::Node
    {
    public:
        Text(XML::Node * Parent, std::string const & Text);
        auto GetText(void) const -> std::string const &;
    private:
        std::string m_Text;
    };
    
    class Document : public XML::Node
    {
    public:
        Document(std::istream & Stream);
        ~Document(void) override;
        auto GetDocumentElement(void) const -> XML::Element const *;
    private:
        XML::Element * m_DocumentElement;
    };
    
    auto IsElement(XML::Node const * Node) -> bool;
    auto IsText(XML::Node const * Node) -> bool;
}

#endif
