#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Enemy
{
public:
    Enemy(sf::Vector2f startPos);

    void update(float dt,
        sf::Vector2f playerPos,
        const std::vector<Enemy>& others);

    void draw(sf::RenderWindow& window);

    void takeDamage(int dmg, sf::Vector2f knockback);

    bool canAttack() const;
    void resetAttack();

    bool isAlive() const;

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;

    // ===== WORLD BOUNDS (STATIKUS) =====
    static void SetWorldBounds(const sf::FloatRect& bounds);

private:
    void updateAnimation();

private:
    sf::Sprite m_sprite;

    std::vector<sf::Texture> m_texturesRight;
    std::vector<sf::Texture> m_texturesLeft;

    sf::Vector2f m_velocity;
    sf::Vector2f m_knockbackVelocity;

    sf::Vector2f m_wanderDir;

    float m_speed;
    float m_visionRange;

    float m_health;
    float m_maxHealth;

    bool m_alive;

    int m_animFrame;

    sf::Clock m_animClock;
    sf::Clock m_attackClock;
    sf::Clock m_wanderClock;

    // ===== STATIKUS WORLD BOUNDS =====
    static sf::FloatRect s_worldBounds;
};
