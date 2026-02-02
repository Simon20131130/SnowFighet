#include "HealthPickup.h"

sf::Texture HealthPickup::s_texture;

HealthPickup::HealthPickup(sf::Vector2f pos)
{
    static bool loaded = false;
    if (!loaded)
    {
        s_texture.loadFromFile("resources/heal.png");
        loaded = true;
    }

    m_sprite.setTexture(s_texture);
    m_sprite.setOrigin(16.f, 16.f);
    m_sprite.setScale(1.5f, 1.5f);
    m_sprite.setPosition(pos);
    m_heal = 2;   // ennyit gyógyít
    m_alive = true;
}

void HealthPickup::draw(sf::RenderWindow& window)
{
    if (m_alive)
        window.draw(m_sprite);
}

bool HealthPickup::isAlive() const
{
    return m_alive;
}

sf::FloatRect HealthPickup::getBounds() const
{
    return m_sprite.getGlobalBounds();
}

int HealthPickup::getHealAmount() const
{
    return m_heal;
}

void HealthPickup::kill()
{
    m_alive = false;
}
