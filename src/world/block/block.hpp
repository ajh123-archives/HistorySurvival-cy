#pragma once

#include "world/block/block_state.hpp"
#include "world/block/block_defs.hpp"
#include "physic/aabb.hpp"

namespace cybrion
{
    struct BlockVertex
    {
        vec3 pos;
        vec2 tex;
        vec3 normal;
        u32 texId;
    };

    struct BlockMesh
    {
        vector<BlockVertex> vertices;
    };

    namespace detail
    {
        template <typename... B>
        class BaseBlocks;
    }

    class Block
    {
    public:
        using Block3x3x3 = array<array<array<Block *, 3>, 3>, 3>;

        Block(const BlockType &type);
        Block(const Block &block) = delete;

        u32 getId() const;
        BlockType getType() const;
        string getDisplayName() const;
        string getSound() const;
        BlockDisplay getDisplay() const;
        BlockShape getShape() const;
        u32 getTexture(u32 index);
        BlockFace getWorldFace(BlockFace face) const;
        BlockRotation getRotationX() const;
        BlockRotation getRotationY() const;
        BlockRotation getRotationZ() const;
        AABB getBound() const;
        vector<ref<BlockMesh>> &getMeshes();
        vector<AABB> &getCollisionBounds();
        u32 getModelTexture(u32 index) const;
        bool isInteractive() const;

        template <typename B>
        B &as()
        {
            return (B &)*this;
        }

        bool operator==(const Block &other) const;

        static BlockFace GetFaceFromDirection(const ivec3 &dir);
        static BlockFace GetOppositeFace(const BlockFace &face);
        static BlockHorizontalFace GetOppositeHorizontalFace(const BlockHorizontalFace &face);
        static ivec3 GetDirectionFromFace(const BlockFace &face);

        virtual string toString() const = 0;

        virtual Block &getPlacedBlock(const ivec3 &pos);
        virtual bool beforePlace(const ivec3 &pos);
        virtual void onPlaced(const ivec3 &pos);
        virtual void onBroken(const ivec3 &pos);
        virtual void onTick(const ivec3 &pos);
        virtual void onInteract(const ivec3 &pos);

        static array<tuple<ivec3, BlockFace>, 6> Directions;

    protected:
        virtual bool checkQuery(const umap<string, string> &query) const = 0;

    private:
        friend class BlockLoader;

        template <typename... B>
        friend class detail::BaseBlocks;

        void rotateBlock()
        {
            auto &f = m_toLocalFace;
            cycleFace(f[1], f[2], f[4], f[5], (u32)m_rotationX);
            cycleFace(f[2], f[0], f[5], f[3], (u32)m_rotationY);
            cycleFace(f[0], f[1], f[3], f[4], (u32)m_rotationZ);

            for (u32 i = 0; i < 6; ++i)
                m_toWorldFace[m_toLocalFace[i]] = i;

            mat4 rotateMat = glm::eulerAngleXYZ(
                u32(m_rotationX) * pi / 2,
                u32(m_rotationY) * pi / 2,
                u32(m_rotationZ) * pi / 2);

            m_bound.rotate({(u32)m_rotationX, (u32)m_rotationY, (u32)m_rotationZ});

            for (auto &bound : m_collisionBounds)
                bound.rotate({(u32)m_rotationX, (u32)m_rotationY, (u32)m_rotationZ});

            if (m_shape == BlockShape::CUSTOM)
            {
                for (auto &mesh : m_meshes)
                {
                    auto temp = mesh;
                    mesh = std::make_shared<BlockMesh>();
                    mesh->vertices = temp->vertices;

                    for (auto &vert : mesh->vertices)
                        vert.pos = rotateMat * vec4(vert.pos, 1);
                }
            }
        }

        void cycleFace(u32 &f1, u32 &f2, u32 &f3, u32 &f4, u32 n)
        {
            while (n--)
            {
                u32 temp = f4;
                f4 = f3;
                f3 = f2;
                f2 = f1;
                f1 = temp;
            }
        }

        u32 m_id;

        BlockDisplay m_display;
        BlockShape m_shape;
        BlockType m_type;
        string m_displayName;

        BlockRotation m_rotationX;
        BlockRotation m_rotationY;
        BlockRotation m_rotationZ;

        string m_sound;

        bool m_isInteractive;

        vector<ref<BlockMesh>> m_meshes;
        vector<u32> m_modelTextures;

        AABB m_bound;
        vector<AABB> m_collisionBounds;

        union
        {
            struct
            {
                u32 m_eastTexture;
                u32 m_topTexture;
                u32 m_southTexture;
                u32 m_westTexture;
                u32 m_bottomTexture;
                u32 m_northTexture;
            };
            u32 m_textures[6];
        };

        u32 m_toWorldFace[6];
        u32 m_toLocalFace[6];
    };

    template <typename B, BlockType type, typename... S>
    class TBlock : public Block, public block::state_holder<S...>
    {
    public:
        TBlock() : Block(type)
        {
        }

        string toString() const override
        {
            string state = block::state_holder<S...>::to_string();

            if (state.empty())
                return "[" + EnumToString(getType()) + "]";

            return "[" + EnumToString(getType()) + ":" + state + "]";
        }

        template <const_string name>
        auto &get() const
        {
            return block::state_holder<S...>::template get<name>();
        }

        template <const_string name>
        B &set(const auto &value);

        // static constexpr u32 num_states()
        // {
        //     return block::state_holder<S...>::num_states();
        // }
        static constexpr BlockType Type = type;

    private:
        template <typename...>
        friend class detail::BaseBlocks;

        bool checkQuery(const umap<string, string> &query) const override
        {
            return block::state_holder<S...>::check_query(query);
        }
    };

    namespace detail
    {
        template <typename B>
        struct block_registry
        {
            static constexpr u32 num_states = B::num_states();
            B blocks[num_states];
        };

        template <typename... B>
        class BaseBlocks : private detail::block_registry<B>...
        {
        public:
            BaseBlocks()
            {
                registerImpl<B...>(0);
            }

            void queryBlocks(BlockType type, const umap<string, string> &query, vector<Block *> &result)
            {
                auto &[from, to] = m_typeToRange[u32(type)];

                for (u32 i = from; i < to; ++i)
                    if (m_idToBlock[i]->checkQuery(query))
                        result.push_back(m_idToBlock[i]);
            }

            template <typename BB>
            BB &get()
            {
                return (BB &)getBlock(BB::Type);
            }

            Block &getBlock(BlockType type)
            {
                // return first block state of this type (all states are 0)
                return getBlock(m_typeToRange[u32(type)].first);
            }

            Block &getBlock(u32 id)
            {
                if (id >= StateCount())
                    return *m_idToBlock[0];

                return *m_idToBlock[id];
            }

            void computeRotation()
            {
                for (u32 i = 0; i < StateCount(); ++i)
                {
                    m_idToBlock[i]->rotateBlock();
                }
            }

            static constexpr u32 StateCount()
            {
                return StateCountImpl<B...>();
            }

        private:
            template <typename T, typename... R>
            static constexpr u32 StateCountImpl()
            {
                if constexpr (sizeof...(R) == 0)
                    return T::num_states();
                else
                    return T::num_states() + StateCountImpl<R...>();
            }

            template <typename T, typename... R>
            void registerImpl(u32 index)
            {
                auto &self = (detail::block_registry<T> &)*this;
                BlockType type = T::Type;

                T block;
                u32 idx = 0;
                T::each_possible_values(block, [&]
                                        {
                    self.blocks[idx] = block;
                    self.blocks[idx].m_id = idx + index;
                    m_idToBlock[idx + index] = &self.blocks[idx];
                    idx += 1; });

                m_typeToRange[u32(type)] = {index, index + idx};

                if constexpr (sizeof...(R) > 0)
                    registerImpl<R...>(index + idx);
            }

            Block *m_idToBlock[StateCount()];
            std::pair<u32, u32> m_typeToRange[u32(BlockType::ENUM_SIZE)];
        };
    }
}