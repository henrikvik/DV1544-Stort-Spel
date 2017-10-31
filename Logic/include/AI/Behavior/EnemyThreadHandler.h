#ifndef ENEMY_THREAD_HANDLER_H
#define ENEMY_THREAD_HANDLER_H

#include <thread>
#include <queue>
#include <mutex>

namespace Logic
{
    class EntityManager;
    class Player;

    class EnemyThreadHandler
    {
    public:
        // Why locked and running? Locked means i % NR_OF_THREADS is locked
        // while running means that index i is currently being updated
        struct WorkData
        {
            EntityManager *manager;
            int index;
            Player const &player;
        };

        enum ThreadStatus
        {
            LOCKED = 0x1,
            RUNNING = 0x2,
            OPEN = 0x4
        };
    private:
        static const int NR_OF_THREADS = 8;

        std::queue<WorkData> m_work;
        std::mutex m_workMutex;

        std::thread *threads[NR_OF_THREADS];
        int m_indexRunning[NR_OF_THREADS];
        bool m_threadRunning[NR_OF_THREADS];

        bool m_killChildren;

        void initThreads();
    public:
        EnemyThreadHandler();
        ~EnemyThreadHandler();

        void updateEnemiesAndPath(WorkData &data);
        void threadMain();

        void addWork(WorkData data);

        void resetThreads();
        void deleteThreads();

        inline int getThreadId(int i)
        {
            return i % NR_OF_THREADS;
        }
        int getThreadStatus(int i);
    };
}

#endif