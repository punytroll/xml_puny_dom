/**
 * galactic-fall
 * Copyright (C) 2006-2022  Hagen Möbius
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
        auto GetChildElements(void) const -> std::vector<XML::Element *>;
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
}

#endif
