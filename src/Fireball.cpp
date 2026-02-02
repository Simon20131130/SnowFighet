#include "Fireball.h"
#include <cmath>

// ===== STATIKUS TEXTÚRA =====
sf::Texture Fireball::s_texture;
bool Fireball::s_loaded = false;

// ===== SEGÉDFÜGGVÉNY =====
static sf::Vector2f normalize(sf::Vector2f v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len == 0.f) return { 0.f, 0.f };
    return v / len;
}

// ===== KONSTRUKTOR =====
Fireball::Fireball(sf::Vector2f pos, sf::Vector2f dir)
{
    if (!s_loaded)
    {
        s_texture.loadFromFile("resources/fireball.png");
        s_loaded = true;
    }

    m_sprite.setTexture(s_texture);
    m_sprite.setOrigin(16.f, 16.f);
    m_sprite.setScale(2.f, 2.f);
    m_sprite.setPosition(pos);

    m_velocity = normalize(dir) * 420.f;
    m_startPos = pos;
}

// ===== UPDATE =====
void Fireball::update(float dt)
{
    if (!m_alive) return;

    m_sprite.move(m_velocity * dt);

    float dist = std::hypot(
        m_sprite.getPosition().x - m_startPos.x,
        m_sprite.getPosition().y - m_startPos.y
    );

    if (dist >= m_maxRange)
        m_alive = false;
}

// ===== DRAW =====
void Fireball::draw(sf::RenderWindow& window) const
{
    if (m_alive)
        window.draw(m_sprite);
}

// ===== STATE =====
bool Fireball::isAlive() const
{
    return m_alive;
}

void Fireball::kill()
{
    m_alive = false;
}

// ===== COLLISION =====
sf::FloatRect Fireball::getBounds() const
{
    return m_sprite.getGlobalBounds();
}
