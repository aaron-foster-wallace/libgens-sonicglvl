#pragma once

namespace LibGens {
    class Mesh;
    class Vertex;
    class VertexFormat;

    class Morph {
        protected:
            string name;
            vector<Vector3> vertex_offsets;

        public:
            void readDescription(File* file);
            void writeDescription(File* file);

            void read(File* file, size_t vertex_count);
            void write(File* file);
    };

    class MorphSet {
        protected:
            vector<Vertex*> vertices;
            vector<Morph*> morphs;
            Mesh* mesh;
            VertexFormat* vertex_format;

        public:
            MorphSet();
            ~MorphSet();

            void fixVertexFormatForPC();

            Mesh* getMesh();

            void read(File* file);
            void write(File* file);
    };
}