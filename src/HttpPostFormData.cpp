#include "../include/HttpPostFormData.hpp"

int HttpPostFormData::size () const {
    return this->contents.size();
}

void HttpPostFormData::clear () {
    this->contents.clear();
}

void HttpPostFormData::addField (const std::string _contentType, 
                                 const std::string _contentName, 
                                 const std::string _content) {
    MultipartContent content;
    content.type = _contentType;
    content.name = _contentName;
    content.data = _content;
    this->contents.push_back(content);
}

bool HttpPostFormData::changeField (const int i,
                                    const std::string _contentType, 
                                    const std::string _contentName, 
                                    const std::string _content) {
    if (i < 0 || this->size() <= i)
        return false;
    this->contents[i].type = _contentType;
    this->contents[i].name = _contentName;
    this->contents[i].data = _content;
    return true;
} 

const std::string& HttpPostFormData::getContentType (const int i) const {
    if (i < 0 || this->size() <= i)
        return "null";
    return this->contents.type;
}

const std::string& HttpPostFormData::getContentName (const int i) const {
    if (i < 0 || this->size() <= i)
        return "null";
    return this->contents.name;
}

const std::string& HttpPostFormData::getContent (const int i) const {
    if (i < 0 || this->size() <= i)
        return "null";
    return this->contents.data;
}