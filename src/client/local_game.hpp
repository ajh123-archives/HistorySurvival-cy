#pragma once

#include "game.hpp"
#include "client/camera.hpp"
#include "client/resource/block_loader.hpp"
#include "client/graphic/block_renderer.hpp"
#include "client/key_code.hpp"
#include "core/stopwatch.hpp"
#include "client/graphic/world_renderer.hpp"
#include "client/graphic/sky_renderer.hpp"

namespace cybrion
{
    constexpr u32 TICKS_PER_SECOND = 20;
    constexpr f32 GAME_TICK = 1000000.0f / TICKS_PER_SECOND;

    using BasicShader = GL::Shader<"MVP">;

    class LocalGame : public Game
    {
    public:
        LocalGame(const string &worldPath);

        void load();
        void tick();

        void render(f32 lerpFactor);
        void renderChunkBorder();
        void renderSelecingBlock();

        void onChunkLoaded(const ref<Chunk> &chunk) override;
        void onChunkUnloaded(const ref<Chunk> &chunk) override;
        void onEntitySpawned(const ref<Entity> &entity) override;
        void onChunkChanged(const ref<Chunk> &result) override;
        void onBreakBlock(const BlockModifyResult &result) override;
        void onPlaceBlock(const BlockModifyResult &result) override;
        void onPlaySound(const string &name) override;

        void onKeyPressed(SDL_Scancode key, bool isRepeated);
        void onKeyReleased(SDL_Scancode key);
        void onMouseMoved(const vec2 &delta);
        void onWindowResized(u32 width, u32 height);
        void onMouseScrolled(f32 delta);

        Camera &getCamera();

        BlockRenderer &getBlockRenderer(u32 id);

        void toggleWireframe();
        void toggleChunkBorder();
        bool showChunkBorder() const;

        Player &getPlayer();

        static LocalGame &Get();

    private:
        friend class ui::GamePage;

        static LocalGame *s_LocalGame;

        void createBlockRenderers();

        Camera m_camera;

        bool m_showWireframe;
        bool m_showChunkBoder;
        bool m_showEntityBorder;
        bool m_showUI;
        bool m_showDebugUI;

        bool m_isFirstMouseMoved;

        Stopwatch m_stopwatch;

        BasicShader m_basicShader;
        GL::Mesh m_chunkBorderMesh;
        GL::Mesh m_targetBlockMesh;

        WorldRenderer m_worldRenderer;
        SkyRenderer m_skyRenderer;
        BlockRenderer m_blockRenderers[Blocks::StateCount()];
    };
}