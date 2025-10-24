/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "json_validator.h"

#include <valijson/adapters/jsoncpp_adapter.hpp>
#include <valijson/utils/jsoncpp_utils.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

bool JsonValidator::LoadAndValidateSchema(const std::string &schema_file, const RemoteSchemaRemap &remote_schema_remap) {
    msg_stream.clear();

    Json::Value schema_document;
    if (!valijson::utils::loadDocument(schema_file.c_str(), schema_document)) {
        msg_stream << "Failed to load schema document.";
        return false;
    }

    const auto fetch_document = [this, &remote_schema_remap](const std::string &uri) -> Json::Value * {
        const std::string schema_path = remote_schema_remap.at(uri);

        auto schema_document = new Json::Value();
        if (!valijson::utils::loadDocument(schema_path.c_str(), *schema_document)) {
            msg_stream << "Failed to load referred schema document:" << uri << std::endl;
            return nullptr;
        }
        return schema_document;
    };

    const auto free_document = [](const Json::Value *adapter) { delete adapter; };

    schema.reset(new valijson::Schema);

    try {
        valijson::SchemaParser parser;
        valijson::adapters::JsonCppAdapter schema_adapter(schema_document);
        parser.populateSchema(schema_adapter, *schema, fetch_document, free_document);
    } catch (const std::runtime_error &error) {
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

bool JsonValidator::ValidateJsonFromFile(const std::string &json_file) {
    Json::Value json_document;
    if (!valijson::utils::loadDocument(json_file.c_str(), json_document)) {
        std::cerr << "Failed to load json document." << std::endl;
        return false;
    }

    return ValidateJson(json_document);
}

bool ValidatePipelineJson(const std::string &json_str) {
    JsonValidator json_validator;

    const std::string schema_path = std::string(SCHEMA_PATH) + "vksc_pipeline_schema.json";

    if (!json_validator.LoadAndValidateSchema(schema_path)) {
        std::cout << "Failed to validate schema:\n" << json_validator.GetMessage() << std::endl;
        return false;
    }

    Json::String err;
    Json::Value json;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size() + 1, &json, &err)) {
        std::cout << "Failed to parse JSON:\n " << json_str << "\n  Parse errors:\n" << err << std::endl;
        return false;
    }

    if (!json_validator.ValidateJson(json)) {
        std::cout << "Failed to validate JSON:\n"
                  << json_str << "\n  Validation errors:\n"
                  << json_validator.GetMessage() << std::endl;
        return false;
    }

    return true;
}

bool ValidatePipelineJsonFile(const char *pipeline_jsonfile) {
    JsonValidator json_validator;

    const std::string schema_path = std::string(SCHEMA_PATH) + "vksc_pipeline_schema.json";

    if (!json_validator.LoadAndValidateSchema(schema_path)) {
        std::cout << "Failed to validate schema:\n" << json_validator.GetMessage() << std::endl;
        return false;
    }

    if (!json_validator.ValidateJsonFromFile(pipeline_jsonfile)) {
        std::cout << "Failed to validate JSON:\n"
                  << pipeline_jsonfile << "\n  Validation errors:\n"
                  << json_validator.GetMessage() << std::endl;
        return false;
    }

    return true;
}

bool CompareJson(const std::string &actual, const std::string &reference) {
    Json::Value json{};

    Json::CharReaderBuilder charReaderBuilder{};
    std::unique_ptr<Json::CharReader> reader(charReaderBuilder.newCharReader());
    Json::String json_errors{};
    if (!reader->parse(reference.c_str(), reference.c_str() + reference.size(), &json, &json_errors)) {
        std::cout << "Failed to parse reference JSON:\n" << reference << "\n  Parse error:\n" << json_errors;
        return false;
    }

    Json::StreamWriterBuilder streamWriterBuilder{};
    auto formatted_ref = Json::writeString(streamWriterBuilder, json);

    if (formatted_ref != actual) {
        std::cout << "Generated JSON mismatch:\n  Reference:\n" << formatted_ref << "\n  Actual:\n" << actual;
        if (getenv("TEST_OUTPUT_JSON_ON_MISMATCH") != nullptr) {
            static uint32_t output_index = 0;
            ++output_index;
            std::ofstream ref(std::string("reference_") + std::to_string(output_index) + ".json");
            ref << formatted_ref;
            ref.close();
            std::ofstream act(std::string("actual_") + std::to_string(output_index) + ".json");
            act << actual;
            act.close();
        }
        return false;
    }

    return true;
}
