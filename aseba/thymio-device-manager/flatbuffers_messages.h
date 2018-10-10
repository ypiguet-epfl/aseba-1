#pragma once
#include <aseba/common/utils/utils.h>
#include <aseba/flatbuffers/thymio_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/flexbuffers.h>
#include "aseba_node.h"
#include "aseba_node_registery.h"
#include "property_flexbuffer.h"
#include <aseba/flatbuffers/fb_message_ptr.h>

namespace mobsya {

using vm_language = fb::ProgrammingLanguage;

tagged_detached_flatbuffer create_nodes_list_request() {
    flatbuffers::FlatBufferBuilder fb;
    auto offset = mobsya::fb::CreateRequestListOfNodes(fb);
    return wrap_fb(fb, offset);
}

tagged_detached_flatbuffer create_error_response(uint32_t request_id, fb::ErrorType error) {
    flatbuffers::FlatBufferBuilder fb;
    auto offset = mobsya::fb::CreateError(fb, request_id, error);
    return wrap_fb(fb, offset);
}

tagged_detached_flatbuffer create_compilation_error_response(uint32_t request_id) {
    flatbuffers::FlatBufferBuilder fb;
    auto offset = mobsya::fb::CreateCompilationError(fb, request_id);
    return wrap_fb(fb, offset);
}

tagged_detached_flatbuffer create_ack_response(uint32_t request_id) {
    flatbuffers::FlatBufferBuilder fb;
    auto offset = mobsya::fb::CreateRequestCompleted(fb, request_id);
    return wrap_fb(fb, offset);
}

tagged_detached_flatbuffer serialize_aseba_vm_description(uint32_t request_id, const mobsya::aseba_node& n,
                                                          const aseba_node_registery::node_id& id) {

    Aseba::TargetDescription desc = n.vm_description();
    flatbuffers::FlatBufferBuilder fb;
    std::vector<flatbuffers::Offset<fb::AsebaNamedVariable>> variables_vector;
    std::vector<flatbuffers::Offset<fb::AsebaEvent>> events_vector;
    std::vector<flatbuffers::Offset<fb::AsebaNativeFunction>> functions_vector;

    int i = 0;
    for(auto&& v : desc.namedVariables) {
        variables_vector.emplace_back(
            fb::CreateAsebaNamedVariable(fb, i++, fb.CreateString(Aseba::WStringToUTF8(v.name)), v.size));
    }

    i = 0;
    for(auto&& v : desc.localEvents) {
        events_vector.emplace_back(fb::CreateAsebaEvent(fb, i++, fb.CreateString(Aseba::WStringToUTF8(v.name)),
                                                        fb.CreateString(Aseba::WStringToUTF8(v.description))));
    }

    i = 0;
    for(auto&& v : desc.nativeFunctions) {
        std::vector<flatbuffers::Offset<fb::AsebaNativeFunctionParameter>> params;
        for(auto&& p : v.parameters) {
            params.emplace_back(
                fb::CreateAsebaNativeFunctionParameter(fb, fb.CreateString(Aseba::WStringToUTF8(p.name)), p.size));
        }
        functions_vector.emplace_back(fb::CreateAsebaNativeFunction(
            fb, i++, fb.CreateString(Aseba::WStringToUTF8(v.name)),
            fb.CreateString(Aseba::WStringToUTF8(v.description)), fb.CreateVector(params)));
    }

    auto offset = CreateNodeAsebaVMDescription(fb, request_id, id.fb(fb), desc.bytecodeSize, desc.variablesSize,
                                               desc.stackSize, fb.CreateVector(variables_vector),
                                               fb.CreateVector(events_vector), fb.CreateVector(functions_vector));
    return wrap_fb(fb, offset);
}

tagged_detached_flatbuffer serialize_changed_variables(const mobsya::aseba_node& n,
                                                       const mobsya::aseba_node::variables_map& vars) {
    flatbuffers::FlatBufferBuilder fb;
    flexbuffers::Builder flexbuilder;
    auto id = n.uuid().fb(fb);
    std::vector<flatbuffers::Offset<fb::NodeVariable>> varsOffsets;
    varsOffsets.reserve(vars.size());
    for(auto&& var : vars) {
        property_to_flexbuffer(var.second, flexbuilder);
        auto& vec = flexbuilder.GetBuffer();
        auto vecOffset = fb.CreateVector(vec);
        auto keyOffset = fb.CreateString(var.first);
        varsOffsets.push_back(fb::CreateNodeVariable(fb, keyOffset, vecOffset));
        flexbuilder.Clear();
    }
    auto offset = fb::CreateNodeVariablesChanged(fb, id, fb.CreateVectorOfSortedTables(&varsOffsets));
    return wrap_fb(fb, offset);
}

mobsya::aseba_node::variables_map variables(const fb::SetNodeVariables& msg) {
    if(!msg.vars())
        return {};
    mobsya::aseba_node::variables_map vars;
    vars.reserve(msg.vars()->size());
    for(const auto& offset : *msg.vars()) {
        if(!offset->name() || !offset->value())
            continue;
        auto k = offset->name()->string_view();
        auto v = offset->value_flexbuffer_root();
        auto p = flexbuffer_to_property(v);
        if(!p)
            continue;
        vars.insert_or_assign(std::string(k), std::move(*p));
    }
    return vars;
}


}  // namespace mobsya
