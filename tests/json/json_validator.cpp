/*
 * Copyright (C) 2022-2025 Valve Corporation
 * Copyright (C) 2022-2025 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "json_validator.h"

#include <valijson/adapters/jsoncpp_adapter.hpp>
#include <valijson/utils/jsoncpp_utils.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

#include <fstream>
#include <cstdarg>
#include <stdio.h>
#include <stdexcept>

bool JsonValidator::LoadAndValidateSchema(const std::string &schema_file, const RemoteSchemaRemap &remote_schema_remap) {
    msg_stream.clear();

    Json::Value schema_document;
    if (!valijson::utils::loadDocument(schema_file.c_str(), schema_document)) {
        msg_stream << "Failed to load schema document.";
        return false;
    }

    const auto fetch_document = [this, &remote_schema_remap](const std::string &uri) -> Json::Value*
    {
        const std::string schema_path = remote_schema_remap.at(uri);

        auto schema_document = new Json::Value();
        if (!valijson::utils::loadDocument(schema_path.c_str(), *schema_document)) {
            msg_stream << "Failed to load referred schema document:" << uri << std::endl;
            return nullptr;
        }
        return schema_document;
    };

    const auto free_document = [](const Json::Value *adapter) {
        delete adapter;
    };
    
    schema.reset(new valijson::Schema);

    try {
        valijson::SchemaParser parser;
        valijson::adapters::JsonCppAdapter schema_adapter(schema_document);
        parser.populateSchema(schema_adapter, *schema, fetch_document, free_document);
    } catch (const std::runtime_error& error) {
        msg_stream << "Failed to parse schema document: " << error.what();
        return false;
    } catch (...) {
        msg_stream << "Failed to parse schema document";
        return false;
    }

    return schema.get() != nullptr;
}

bool JsonValidator::ValidateJson(const Json::Value &json_document) {
    assert(!json_document.empty());

    valijson::Validator validator(valijson::Validator::kStrongTypes);
    valijson::adapters::JsonCppAdapter document_adapter(json_document);

    valijson::ValidationResults results;
    msg_stream.clear();

    if (!validator.validate(*schema, document_adapter, &results)) {
        valijson::ValidationResults::Error error;
        uint32_t error_num = 1;
        while (results.popError(error)) {
            std::string context;
            std::vector<std::string>::iterator itr = error.context.begin();
            for (; itr != error.context.end(); itr++) {
                context += *itr;
            }

            msg_stream << "Error #" << error_num << "\n";
            msg_stream << "\t context: " + context + "\n";
            msg_stream << "\t desc:    " + error.description + "\n\n";

            ++error_num;
        }

        msg_stream << "Total Error Count: " << error_num << "\n";

        return false;
    }
    return true;
}

bool JsonValidator::ValidateJsonFromFile(const std::string &json_file)
{
    Json::Value json_document;
    if (!valijson::utils::loadDocument(json_file.c_str(), json_document)) {
        std::cerr << "Failed to load json document." << std::endl;
        return false;
    }

    return ValidateJson(json_document);
}

