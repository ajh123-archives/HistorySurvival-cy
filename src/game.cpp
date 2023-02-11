#include "game.hpp"
#include "world/block/nature/log_block.hpp"
#include "world/block/nature/air_block.hpp"

namespace cybrion
{
    Game* Game::s_game = nullptr;

    Game::Game()
    {
        s_game = this;
    }

    void Game::load()
    {
        // register blocks
        m_blockRegistry.load();

        // load block configs
        m_blockLoader.load();

        // precompute some block properties
        m_blockRegistry.computeRotation();

        m_world.loadChunk({ 0, 0, 0 });
    }

    void Game::tick()
    {
    }

    BlockRegistry& Game::getBlockRegistry()
    {
        return m_blockRegistry;
    }

    BlockLoader& Game::getBlockLoader()
    {
        return m_blockLoader;
    }

    World& Game::getWorld()
    {
        return m_world;
    }

    Game& Game::Get()
    {
        return *s_game;
    }
}
