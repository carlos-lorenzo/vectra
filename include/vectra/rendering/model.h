#ifndef VECTRA_MODEL_H
#define VECTRA_MODEL_H
#include <vector>
#include <string>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "vectra/rendering/texture.h"
#include "vectra/rendering/shader.h"
#include "vectra/rendering/mesh.h"


class Model
{
    public:
        // model data
        std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Mesh> meshes;
        std::string directory;

        bool gamma_correction;

        Model(std::string const &path, bool gamma = false);
        void draw(Shader &shader);
    private:
        void load_model(std::string const &path);
        void process_node(aiNode *node, const aiScene *scene);
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, std::string type_name);
};
#endif //VECTRA_MODEL_H