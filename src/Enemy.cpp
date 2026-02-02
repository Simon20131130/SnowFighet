#include "Enemy.h"
#include <cmath>
#include <algorithm>

sf::FloatRect Enemy::s_worldBounds;

static sf::Vector2f normalize(sf::Vector2f v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len == 0.f) return { 0.f, 0.f };
    return v / len;
}

void Enemy::SetWorldBounds(const sf::FloatRect& bounds)
{
    s_worldBounds = bounds;
}

Enemy::Enemy(sf::Vector2f startPos)
{
    m_texturesRight.resize(3);
    m_texturesLeft.resize(3);

    m_texturesRight[0].loadFromFile("resources/snowman1.png");
    m_texturesRight[1].loadFromFile("resources/snowman2.png");
    m_texturesRight[2].loadFromFile("resources/snowman3.png");

    m_texturesLeft[0].loadFromFile("resources/snowman4.png");
    m_texturesLeft[1].loadFromFile("resources/snowman5.png");
    m_texturesLeft[2].loadFromFile("resources/snowman6.png");

    m_sprite.setTexture(m_texturesRight[0]);
    m_sprite.setOrigin(16.f, 16.f);
    m_sprite.setScale(3.5f, 3.5f);
    m_sprite.setPosition(startPos);

    m_animFrame = 0;

    m_speed = 130.f;
    m_visionRange = 600.f;

    m_maxHealth = 4.f;
    m_health = m_maxHealth;

    m_knockbackVelocity = { 0.f, 0.f };

    m_alive = true;
    m_wanderDir = { 1.f, 0.f };
}

void Enemy::update(float dt,
    sf::Vector2f playerPos,
    const std::vector<Enemy>& others)
{
    if (!m_alive) return;

    sf::Vector2f pos = m_sprite.getPosition();
    sf::Vector2f moveDir(0.f, 0.f);

    float distToPlayer = std::hypot(
        playerPos.x - pos.x,
        playerPos.y - pos.y
    );

    // ======================
    // HA PÁLYÁN KÍVÜL VAN → MENJEN BE
    // ======================
    if (!s_worldBounds.contains(pos))
    {
        sf::Vector2f center(
            s_worldBounds.left + s_worldBounds.width * 0.5f,
            s_worldBounds.top + s_worldBounds.height * 0.5f
        );

        moveDir = normalize(center - pos) * 1.4f; // gyors belépés
    }
    else
    {
        // ======================
        // CHASE
        // ======================
        if (distToPlayer < m_visionRange)
        {
            moveDir += normalize(playerPos - pos);
        }
        else
        {
            // ======================
            // WANDER
            // ======================
            if (m_wanderClock.getElapsedTime().asSeconds() > 2.f)
            {
                m_wanderClock.restart();

                float angle =
                    static_cast<float>(rand()) / RAND_MAX * 6.28318f;

                m_wanderDir = {
                    std::cos(angle),
                    std::sin(angle)
                };
            }

            moveDir += m_wanderDir * 0.4f;
        }

        // ======================
        // SEPARATION
        // ======================
        for (const Enemy& e : others)
        {
            if (&e == this) continue;

            sf::Vector2f diff = pos - e.getPosition();
            float d = std::hypot(diff.x, diff.y);

            if (d > 0.f && d < 110.f)
                moveDir += normalize(diff) * (110.f - d) * 0.02f;
        }
    }

    if (moveDir.x != 0.f || moveDir.y != 0.f)
        moveDir = normalize(moveDir);

    // ======================
    // VELOCITY + KNOCKBACK
    // ======================
    m_velocity = moveDir * m_speed;

    m_knockbackVelocity *= 0.85f;
    m_velocity += m_knockbackVelocity;

    sf::Vector2f newPos = pos + m_velocity * dt;

    // ======================
    // HARD CLAMP – SOHA KI
    // ======================
    const float halfSize = 56.f; // sprite ~112px

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

void Enemy::updateAnimation()
{
    if (m_animClock.getElapsedTime().asSeconds() < 0.2f)
        return;

    m_animClock.restart();
    m_animFrame = (m_animFrame + 1) % 3;

    if (m_velocity.x >= 0.f)
        m_sprite.setTexture(m_texturesRight[m_animFrame]);
    else
        m_sprite.setTexture(m_texturesLeft[m_animFrame]);
}

void Enemy::draw(sf::RenderWindow& window)
{
    if (!m_alive) return;

    window.draw(m_sprite);

    // -------- HEALTH BAR --------
    float hpRatio = m_health / m_maxHealth;

    sf::RectangleShape bg({ 30.f, 5.f });
    bg.setFillColor(sf::Color(50, 50, 50));
    bg.setOrigin(15.f, 0.f);
    bg.setPosition(
        m_sprite.getPosition().x,
        m_sprite.getPosition().y - 55.f
    );

    sf::RectangleShape hp({ 30.f * hpRatio, 5.f });
    hp.setFillColor(sf::Color(200, 60, 60));
    hp.setOrigin(15.f, 0.f);
    hp.setPosition(bg.getPosition());

    window.draw(bg);
    window.draw(hp);
}

void Enemy::takeDamage(int dmg, sf::Vector2f knockback)
{
    m_health -= dmg;
    m_knockbackVelocity += normalize(knockback) * 300.f;

    if (m_health <= 0)
        m_alive = false;
}

bool Enemy::canAttack() const
{
    return m_attackClock.getElapsedTime().asSeconds() > 1.f;
}

void Enemy::resetAttack()
{
    m_attackClock.restart();
}

bool Enemy::isAlive() const
{
    return m_alive;
}

sf::FloatRect Enemy::getBounds() const
{
    return m_sprite.getGlobalBounds();
}

sf::Vector2f Enemy::getPosition() const
{
    return m_sprite.getPosition();
}
