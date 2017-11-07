#ifndef ENEMY_H
#define ENEMY_H

#include <Entity\Entity.h>
#include <AI\EnemyType.h>

#pragma region Comment
/*
	CLASS: Enemy
	AUTHOR: Lukas Westling

	This class handles the Enemy and
	its flags is handled by EntityManager

	But wait? Why cant i check if an enemy is dead???
	That is because that is very slow and waste of cachelines!

	! changing this = bad performence !
*/
#pragma endregion ClassDesc

namespace Logic 
{
    class Player;
    class Behavior;

	class Enemy : public Entity 
	{
		private:
            // This is used to count how many callbacks is added, 
            // so this HAS to be ZERO before this is destroyed or
            // the showcase at PAX East will go bad
            int m_nrOfCallbacksEntities;

            // base
            int m_health, m_maxHealth, m_baseDamage;
            float m_moveSpeed;

			float m_bulletTimeMod;									// Variables for effect modifiers
            float m_moveSpeedMod;
			ENEMY_TYPE m_enemyType;

			Behavior *m_behavior;

            //status
            bool m_stunned;
		public:	
			enum BEHAVIOR_ID { TEST, RANGED, MELEE };

			Enemy(Graphics::ModelID modelID, btRigidBody* body, btVector3 halfExtent, int maxHealth, int baseDamage, float moveSpeed, ENEMY_TYPE enemyType, int animationId);
			virtual ~Enemy();

			virtual void update(Player const &player, float deltaTime,
				std::vector<Enemy*> const &closeEnemies);
			virtual void useAbility(Entity const &target) {};
			virtual void updateDead(float deltaTime) = 0;
			virtual void updateSpecific(Player const &player, float deltaTime) = 0;

			virtual void affect(int stacks, Effect const &effect, float dt);
            void onEffectEnd(int stacks, Effect const &effect);

			Projectile* shoot(btVector3 dir, Graphics::ModelID id, float speed, float gravity, float scale);

			// for debugging
			void debugRendering(Graphics::Renderer &renderer);

            void increaseCallbackEntities();
            void decreaseCallbackEntities();
            bool hasCallbackEntities();

			void damage(int damage);
			void setBehavior(BEHAVIOR_ID id);
			void setEnemyType(ENEMY_TYPE id);

            int getHealth() const;
            int getMaxHealth() const;
            int getBaseDamage() const;

			float getMoveSpeed() const;
			ENEMY_TYPE getEnemyType() const;
			Behavior* getBehavior() const;
	};
}

#endif