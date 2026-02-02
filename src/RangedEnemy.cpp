#include "RangedEnemy.h"
#include <cmath>
#include <algorithm>

// ===== statikus textúrák =====
sf::Texture RangedEnemy::s_idle;
sf::Texture RangedEnemy::s_right1;
sf::Texture RangedEnemy::s_right2;
sf::Texture RangedEnemy::s_left1;
sf::Texture RangedEnemy::s_left2;
sf::Texture RangedEnemy::s_throw;

sf::FloatRect RangedEnemy::s_worldBounds;

static sf::Vector2f normalize(sf::Vector2f v)
{
    float l = std::sqrt(v.x * v.x + v.y * v.y);
    if (l == 0.f) return { 0.f, 0.f };
    return v / l;
}

void RangedEnemy::SetWorldBounds(const sf::FloatRect& bounds)
{
    s_worldBounds = bounds;
}

RangedEnemy::RangedEnemy(sf::Vector2f pos)
{
    static bool loaded = false;
    if (!loaded)
    {
        s_idle.loadFromFile("resources/elf1.png");
        s_right1.loadFromFile("resources/elf2.png");
        s_right2.loadFromFile("resources/elf3.png");
        s_throw.loadFromFile("resources/elf4.png");
        s_left1.loadFromFile("resources/elf5.png");
        s_left2.loadFromFile("resources/elf6.png");
        loaded = true;
    }

    m_sprite.setTexture(s_idle);
    m_sprite.setOrigin(16.f, 16.f);
    m_sprite.setScale(3.5f, 3.5f);
    m_sprite.setPosition(pos);

    m_speed = 90.f;
    m_visionRange = 1200.f;

    m_maxHealth = 3;
    m_health = m_maxHealth;

    m_alive = true;
    m_throwing = false;
    m_animFrame = 0;
}

void RangedEnemy::update(float dt,
    sf::Vector2f playerPos,
    std::vector<Fireball>& fireballs)
{
    if (!m_alive) return;

    sf::Vector2f pos = m_sprite.getPosition();
    sf::Vector2f toPlayer = playerPos - pos;
    float dist = std::hypot(toPlayer.x, toPlayer.y);

    m_velocity = { 0.f, 0.f };

    // ======================
    // HA PÁLYÁN KÍVÜL → MENJEN BE
    // ======================
    if (!s_worldBounds.contains(pos))
    {
        sf::Vector2f center(
            s_worldBounds.left + s_worldBounds.width * 0.5f,
            s_worldBounds.top + s_worldBounds.height * 0.5f
        );

        m_velocity = normalize(center - pos) * (m_speed * 1.4f);
    }
    else
    {
        // ======================
        // KITING LOGIKA
        // ======================
        if (dist < 300.f)
        {
            m_velocity = -normalize(toPlayer) * m_speed;
        }
        else if (dist > 700.f && dist < m_visionRange)
        {
            m_velocity = normalize(toPlayer) * m_speed;
        }

        // ======================
        // LÖVÉS
        // ======================
        if (!m_throwing &&
            dist < m_visionRange &&
            m_throwClock.getElapsedTime().asSeconds() > 1.5f)
        {
            m_throwClock.restart();
            m_throwing = true;
            m_throwAnimClock.restart();

            m_sprite.setTexture(s_throw);
            fireballs.emplace_back(pos, toPlayer);
        }
    }

    sf::Vector2f newPos = pos + m_velocity * dt;

    // ======================
    // HARD CLAMP – SOHA KI
    // ======================
    const float halfSize = 56.f;

    newPos.x = std::clamp(
        newPos.x,
        s_worldBounds.left + halfSize,
        s_worldBounds.left + s_worldBounds.width - halfSize
    );

    newPos.y = std::clamp(
        newPos.y,
        s_worldBounds.top + halfSize,
        s_worldBounds.top + s_worldBounds.height - halfSize
    );

    m_sprite.setPosition(newPos);

    updateAnimation();
}

void RangedEnemy::updateAnimation()
{
    // dobás anim maradjon
    if (m_throwing)
    {
        if (m_throwAnimClock.getElapsedTime().asSeconds() < 0.45f)
            return;

        m_throwing = false;
    }

    if (m_animClock.getElapsedTime().asSeconds() < 0.25f)
        return;

    m_animClock.restart();
    m_animFrame = 1 - m_animFrame;

    if (m_velocity.x > 10.f)
        m_sprite.setTexture(m_animFrame == 0 ? s_right1 : s_right2);
    else if (m_velocity.x < -10.f)
        m_sprite.setTexture(m_animFrame == 0 ? s_left1 : s_left2);
    else
        m_sprite.setTexture(s_idle);
}

void RangedEnemy::draw(sf::RenderWindow& window)
{
    if (!m_alive) return;

    window.draw(m_sprite);

    // ===== HP BAR =====
    float ratio =
        static_cast<float>(m_health) /
        static_cast<float>(m_maxHealth);

    sf::Vector2f pos = m_sprite.getPosition();

    sf::RectangleShape bg({ 50.f, 6.f });
    bg.setFillColor(sf::Color(40, 40, 40));
    bg.setPosition(pos.x - 25.f, pos.y - 80.f);

    sf::RectangleShape bar({ 50.f * ratio, 6.f });
    bar.setFillColor(sf::Color(220, 80, 80));
    bar.setPosition(bg.getPosition());

    window.draw(bg);
    window.draw(bar);
}

void RangedEnemy::takeDamage(int dmg)
{
    m_health -= dmg;
    if (m_health <= 0)
        m_alive = false;
}

bool RangedEnemy::isAlive() const
{
    return m_alive;
}

sf::FloatRect RangedEnemy::getBounds() const
{
    return m_sprite.getGlobalBounds();
}

sf::Vector2f RangedEnemy::getPosition() const
{
    return m_sprite.getPosition();
}

int RangedEnemy::getHealth() const { return m_health; }
int RangedEnemy::getMaxHealth() const { return m_maxHealth; }
