#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Fireball.h"

class RangedEnemy
{
public:
    RangedEnemy(sf::Vector2f pos);

    void update(float dt,
        sf::Vector2f playerPos,
        std::vector<Fireball>& fireballs);

    void draw(sf::RenderWindow& window);

    void takeDamage(int dmg);

    bool isAlive() const;

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;

    int getHealth() const;
    int getMaxHealth() const;

    // ===== WORLD BOUNDS =====
    static void SetWorldBounds(const sf::FloatRect& bounds);

private:
    void updateAnimation();

private:
    sf::Sprite m_sprite;

    sf::Vector2f m_velocity;

    float m_speed;
    float m_visionRange;

    int m_health;
    int m_maxHealth;

    bool m_alive;
    bool m_throwing;

    int m_animFrame;

    sf::Clock m_animClock;
    sf::Clock m_throwClock;
    sf::Clock m_throwAnimClock;

    // ===== STATIKUS TEXTÚRÁK =====
    static sf::Texture s_idle;
    static sf::Texture s_right1;
    static sf::Texture s_right2;
    static sf::Texture s_left1;
    static sf::Texture s_left2;
    static sf::Texture s_throw;

    // ===== WORLD BOUNDS =====
    static sf::FloatRect s_worldBounds;
};
