#include "Mesh.h"
#include "Morph.h"
#include "Submesh.h"
#include "Vertex.h"
#include "VertexFormat.h"

namespace LibGens {
    void Morph::readDescription(File* file) {
        file->readString(&name);
        printf("%s\n", name.c_str());
    }

    void Morph::writeDescription(File* file) {
        file->writeString(&name);
    }

    void Morph::read(File* file, size_t vertex_count) {
        vertex_offsets.reserve(vertex_count);
        for (size_t i = 0; i < vertex_count; i++) {
            Vector3 vertex_offset;
            vertex_offset.read(file);

            vertex_offsets.push_back(vertex_offset);
        }
    }

    void Morph::write(File* file) {
        for (auto it = vertex_offsets.begin(); it != vertex_offsets.end(); ++it)
            (*it).write(file);
    }

    MorphSet::MorphSet() : mesh(NULL), vertex_format(NULL) {
    }

    MorphSet::~MorphSet() {
        for (auto it = vertices.begin(); it != vertices.end(); ++it)
            delete* it;

        for (auto it = morphs.begin(); it != morphs.end(); ++it)
            delete* it;

        delete mesh;
    }

    void MorphSet::fixVertexFormatForPC() {
        vertex_format->fixForPC();
    }

    Mesh* MorphSet::getMesh() {
        return mesh;
    }

    void MorphSet::read(File* file) {
        unsigned int vertex_count = 0;
        size_t vertices_address = 0;
        unsigned int morph_count = 0;
        size_t morph_names_address = 0;
        size_t morphs_address = 0;

        file->readInt32BE(&vertex_count);
        file->readInt32BEA(&vertices_address);
        file->moveAddress(4);
        file->readInt32BE(&morph_count);
        file->readInt32BEA(&morph_names_address);
        file->readInt32BEA(&morphs_address);

        mesh = new Mesh();
        mesh->read(file);

        // Pick the first vertex format, they are all the same between submeshes
        for (size_t i = 0; i < LIBGENS_MODEL_SUBMESH_SLOTS; i++) {
            vector<Submesh*> submeshes = mesh->getSubmeshes(i);
            for (auto it = submeshes.begin(); it != submeshes.end(); ++it) {
                vertex_format = (*it)->getVertexFormat();
                break;
            }

            if (vertex_format)
                break;
        }

        vertices.reserve(vertex_count);
        for (size_t i = 0; i < vertex_count; i++) {
            file->goToAddress(vertices_address + i * vertex_format->getSize());

            Vertex* vertex = new Vertex();
            vertex->read(file, vertex_format);
            vertices.push_back(vertex);
        }

        size_t address;

        morphs.reserve(morph_count);
        for (size_t i = 0; i < morph_count; i++) {
            file->goToAddress(morph_names_address + i * 4);
            file->readInt32BEA(&address);

            file->goToAddress(address);

            Morph* morph = new Morph();
            morph->readDescription(file);
            morphs.push_back(morph);
        }

        for (size_t i = 0; i < morph_count; i++) {
            file->goToAddress(morphs_address + i * 4);
            file->readInt32BEA(&address);

            file->goToAddress(address);
            morphs[i]->read(file, vertex_count);
        }
    }

    void MorphSet::write(File* file) {
        size_t header_address = file->getCurrentAddress();

        unsigned int vertex_count = vertices.size();
        size_t vertices_address = 0;
        unsigned int unknown = 1;
        unsigned int morph_count = morphs.size();
        size_t morph_names_address = 0;
        size_t morphs_address = 0;

        file->writeInt32BE(&vertex_count);
        file->writeNull(4);
        file->writeInt32BE(&unknown);
        file->writeInt32BE(&morph_count);
        file->writeNull(8);

        for (size_t i = 0; i < LIBGENS_MODEL_SUBMESH_SLOTS; i++) {
            vector<Submesh*> submeshes = mesh->getSubmeshes(i);
            for (auto it = submeshes.begin(); it != submeshes.end(); ++it)
                (*it)->setVertexFormat(vertex_format);
        }

        mesh->write(file);

        vertices_address = file->getCurrentAddress();
        for (auto it = vertices.begin(); it != vertices.end(); ++it)
            (*it)->write(file, vertex_format);

        file->fixPadding();

        vector<size_t> addresses;

        morph_names_address = file->getCurrentAddress();
        file->writeNull(morph_count * 4);

        for (auto it = morphs.begin(); it != morphs.end(); ++it) {
            addresses.push_back(file->getCurrentAddress());
            (*it)->writeDescription(file);
        }

        file->fixPadding();

        for (size_t i = 0; i < morph_count; i++) {
            file->goToAddress(morph_names_address + i * 4);
            file->writeInt32BEA(&addresses[i]);
        }
        file->goToEnd();

        morphs_address = file->getCurrentAddress();
        file->writeNull(morph_count * 4);

        addresses.clear();
        for (auto it = morphs.begin(); it != morphs.end(); ++it) {
            addresses.push_back(file->getCurrentAddress());
            (*it)->write(file);
        }

        file->fixPadding();

        for (size_t i = 0; i < morph_count; i++) {
            file->goToAddress(morphs_address + i * 4);
            file->writeInt32BEA(&addresses[i]);
        }

        file->goToAddress(header_address + 4);
        file->writeInt32BEA(&vertices_address);
        file->moveAddress(8);
        file->writeInt32BEA(&morph_names_address);
        file->writeInt32BEA(&morphs_address);
        file->goToEnd();
    }
}