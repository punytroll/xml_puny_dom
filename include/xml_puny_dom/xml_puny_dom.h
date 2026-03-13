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
#include <map>
#include <string>
#include <vector>
#include <ranges>

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
    class Node;
    
    auto IsElement(XML::Node const * Node) -> bool;
    auto IsText(XML::Node const * Node) -> bool;
    
    class Node
    {
    public:
        Node(XML::NodeType NodeType, XML::Node * ParentNode, std::uint64_t SourceLine, std::uint64_t SourceColumn);
        virtual ~Node();
        auto GetChildElements() const;
        auto GetChildNodes() const -> std::vector<XML::Node *> const &;
        auto GetChildNode(std::vector<XML::Node *>::size_type Index) const -> XML::Node const *;
        auto GetChildTexts() const;
        auto GetNodeType() const -> XML::NodeType;
        auto GetParentNode() const -> XML::Node const *;
        auto GetSourceColumn() const -> std::uint64_t;
        auto GetSourceLine() const -> std::uint64_t;
    protected:
        std::vector<XML::Node *> m_ChildNodes;
        XML::NodeType m_NodeType;
        XML::Node * m_ParentNode;
        std::uint64_t m_SourceColumn;
        std::uint64_t m_SourceLine;
    };
    
    class Element : public XML::Node
    {
    public:
        Element(XML::Node * Parent, std::uint64_t SourceLine, std::uint64_t SourceColumn, std::string const & Name, std::map<std::string, std::string> const & Attributes);
        auto GetName() const -> std::string const &;
        auto GetAttributes() const -> std::map<std::string, std::string> const &;
        auto GetAttribute(std::string const & AttributeName) const -> std::string const &;
        auto HasAttribute(std::string const & AttributeName) const -> bool;
    private:
        std::string m_Name;
        std::map<std::string, std::string> m_Attributes;
    };
    
    class Text : public XML::Node
    {
    public:
        Text(XML::Node * Parent, std::uint64_t SourceLine, std::uint64_t SourceColumn, std::string const & Text);
        auto GetText() const -> std::string const &;
    private:
        std::string m_Text;
    };
    
    class Document : public XML::Node
    {
    public:
        Document(std::istream & Stream);
        ~Document() override;
        auto GetDocumentElement() const -> XML::Element const *;
    private:
        XML::Element * m_DocumentElement;
    };
}

inline auto XML::Node::GetChildElements() const
{
    return m_ChildNodes | std::views::filter(XML::IsElement) | std::views::transform([](XML::Node const * Node)
                                                                                     {
                                                                                         return dynamic_cast<XML::Element const *>(Node);
                                                                                     });
}

inline auto XML::Node::GetChildTexts() const
{
    return m_ChildNodes | std::views::filter(XML::IsText) | std::views::transform([](XML::Node const * Node)
                                                                                  {
                                                                                      return dynamic_cast<XML::Text const *>(Node);
                                                                                  });
}

#endif
