/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 * Copyright (c) 2022-2025 Valve Corporation
 * Copyright (c) 2022-2025 LunarG, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <json/json.h>
#include <valijson/schema.hpp>
#include <memory>
#include <string>
#include <unordered_map>

class JsonValidator {
  public:
    using RemoteSchemaRemap = std::unordered_map<std::string, std::string>;

    JsonValidator() = default;
    ~JsonValidator() = default;

    bool LoadAndValidateSchema(const std::string &schema_file, const RemoteSchemaRemap &remote_schema_remap = {});

    bool ValidateJson(const Json::Value &json_document);
    bool ValidateJsonFromFile(const std::string &json_file);

    std::string GetMessage() const { return msg_stream.str(); }

  private:
    std::stringstream msg_stream;
    std::unique_ptr<valijson::Schema> schema;
};
