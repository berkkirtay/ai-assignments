// Copyright (c) 2022 Berk Kırtay

#include <vector>
#include <iostream>
#include <cmath>
#include <stack>
#include <queue>
#include <algorithm>
#include <random>
#include <chrono>
#include <memory>

#define FREE -1
#define WALL -2
#define PEG 1

using namespace std;
/*
 * SoloTest board initializator class
 */
class SoloTest
{
public:
    vector<vector<int>> board;
    // Possible moves from a free block:
    vector<pair<int, int>> moves = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};
    void createBoard()
    {
        if (board.empty() == false)
        {
            board.clear();
        }
        // FREE represents the free pegs and WALL represents the unreachable indexes:
        board.push_back({WALL, WALL, PEG, PEG, PEG, WALL, WALL});
        board.push_back({WALL, WALL, PEG, PEG, PEG, WALL, WALL});
        board.push_back({PEG, PEG, PEG, PEG, PEG, PEG, PEG});
        board.push_back({PEG, PEG, PEG, FREE, PEG, PEG, PEG});
        board.push_back({PEG, PEG, PEG, PEG, PEG, PEG, PEG});
        board.push_back({WALL, WALL, PEG, PEG, PEG, WALL, WALL});
        board.push_back({WALL, WALL, PEG, PEG, PEG, WALL, WALL});
    }
};
/*
 * Nodes consist of a board state representation, a parent and
 * a last deleted peg index for frontier ordering for the first 3 methods.
 */
class Node
{
public:
    shared_ptr<Node> parent;
    vector<vector<int>> board;
    pair<int, int> lastDeletedPegIndex = {0, 0};
    // The initial board has only one free block so its score will be 1.
    double score = 1;
    Node(vector<vector<int>> board)
    {
        this->board = board;
    }
};

/*
 * NodeSelection class determines how
 * the nodes are selected from the frontier.
 * This can be numbered order, random or heuristic.
 */
class NodeSelection
{
public:
    virtual vector<shared_ptr<Node>> select(vector<shared_ptr<Node>> &nodes) = 0;
};

class IndexFirst : public NodeSelection
{
    vector<shared_ptr<Node>> select(vector<shared_ptr<Node>> &nodes)
    {
        stable_sort(nodes.begin(), nodes.end(),
                    [](const auto &l, const auto &r)
                    {
                        if (l->lastDeletedPegIndex.first == r->lastDeletedPegIndex.first)
                        {
                            return l->lastDeletedPegIndex.second > r->lastDeletedPegIndex.second;
                        }
                        else
                        {
                            return l->lastDeletedPegIndex.first > r->lastDeletedPegIndex.first;
                        }
                    });
        return nodes;
    }
};

class Random : public NodeSelection
{
    vector<shared_ptr<Node>> select(vector<shared_ptr<Node>> &nodes)
    {
        shuffle(nodes.begin(), nodes.end(), default_random_engine{random_device{}()});
        return nodes;
    }
};

/*
 * Our Heuristic class handles the frontier selection for
 * the DFS with a Node Selection Heuristic method.
 * The lower heuristic score is better chance to be selected in the frontier.
 * All the heuristic scores of the frontier nodes are then sorted and some of them
 * are to be neglected to prune the tree search space. Below, other details are explained:
 */
class Heuristic : public NodeSelection
{
    int calculateBoardHeuristic(shared_ptr<Node> &node)
    {
        int heuristic = 0;

        for (int i = 0; i < node->board.size(); i++)
        {
            for (int j = 0; j < node->board[i].size(); j++)
            {
                if (node->board[i][j] == PEG)
                {
                    /*
                     * Calculating the squared oclidean distances of the pegs
                     * to the center (closer is better):
                     */
                    heuristic += pow(i - 3, 2) + pow(j - 3, 2);

                    /*
                     * The pegs in the corners should be avoided.Because
                     * the pegs in the corners cannot be removed easily.
                     */
                    if (i == 0 && j == 3 || i == 0 && j == 5 ||
                        i == 3 && j == 0 || i == 5 && j == 0 ||
                        i == 3 && j == 8 || i == 5 && j == 8 ||
                        i == 8 && j == 3 || i == 8 && j == 5)
                    {
                        heuristic += 2;
                    }

                    /*
                     * Checking if the blocks around center have a peg or not.
                     * This helps to get to the optimal solution by checking if
                     * any of the center blocks have one peg to handle the final move
                     * towards to the center block.
                     */
                    if (i == 3 && j == 3 || i == 1 && j == 3 ||
                        i == 3 && j == 1 || i == 3 && j == 5 ||
                        i == 5 && j == 3)
                    {
                        heuristic -= 10;
                    }
                }
            }
        }

        // Adding the score of the node to the heuristic:
        heuristic -= node->score;
        return heuristic;
    }

    /*
     * We select the nodes with higher numbers of emtpy blocks.
     * So, we create a priority queue for every move state in every depth:
     */
    vector<shared_ptr<Node>> select(vector<shared_ptr<Node>> &nodes)
    {
        vector<pair<int, shared_ptr<Node>>> prioritizedMoves;
        vector<shared_ptr<Node>> res;

        for (auto node : nodes)
        {
            auto score = calculateBoardHeuristic(node);
            prioritizedMoves.push_back(make_pair(score, node));
        }
        // To reverse the array later, we sort the array increasingly:
        sort(prioritizedMoves.begin(), prioritizedMoves.end(),
             [](const auto &l, const auto &r)
             {
                 return l.first < r.first;
             });

        /* We do not include the unpromising boards to the frontier by
         * neglecting the lower half of the acquired moves:
         * This provides cutting the search space by half for every new move search.
         */
        int counter = 0;
        while (prioritizedMoves.empty() == false)
        {
            auto &node = prioritizedMoves.back().second;
            counter++;
            if (counter > prioritizedMoves.size() / 2)
            {
                res.push_back(node);
            }
            prioritizedMoves.pop_back();
        }
        return res;
    }
};

/*
 * Frontier class is a base class for both
 * Queue and Stack which is used by DFS and BFS
 * algorithms. Queue and Stack classes are wrappers
 * around stl queue and stack classes.
 */
class Frontier
{
public:
    virtual bool empty() = 0;
    virtual size_t size() = 0;
    virtual void push(shared_ptr<Node> node) = 0;
    virtual shared_ptr<Node> pop_return() = 0;
};

class Stack : public Frontier
{
    stack<shared_ptr<Node>> s;

public:
    bool empty()
    {
        return s.empty();
    }
    size_t size()
    {
        return s.size();
    }
    void push(shared_ptr<Node> node)
    {
        s.push(node);
    }
    shared_ptr<Node> pop_return()
    {
        auto ret = s.top();
        s.pop();
        return ret;
    }
};

class Queue : public Frontier
{
    queue<shared_ptr<Node>> q;

public:
    bool empty()
    {
        return q.empty();
    }
    size_t size()
    {
        return q.size();
    }
    void push(shared_ptr<Node> node)
    {
        q.push(node);
    }
    shared_ptr<Node> pop_return()
    {
        auto ret = q.front();
        q.pop();
        return ret;
    }
};

/*
 * Solver class provides a problem solver framework for all 5 methods.
 * It saves best found board, checks if stop condition is raised and the
 * other required things that are mentioned in the homework.
 */
class Solver
{
public:
    SoloTest st;
    shared_ptr<Node> bestNode = nullptr;
    int timeLimit = 0;
    int maxScore = 0;
    bool stop = false;
    bool isDepthLimitReached = false;
    int numberOfExpandedNodes = 0;
    int maxNumberOfStoredNodes = 0;

    Solver(int timeLimit)
    {
        st.createBoard();
        this->timeLimit = timeLimit;
    }

    /*
     * This function searches possible moves on a board state and returns them
     * by assigning their parent node as the given board.
     */
    vector<shared_ptr<Node>> searchMoves(shared_ptr<Node> &board)
    {
        vector<shared_ptr<Node>> nodes;
        vector<vector<int>> tempBoard = board->board;
        for (int i = 0; i < board->board.size(); i++)
        {
            for (int j = 0; j < board->board[i].size(); j++)
            {
                if (board->board[i][j] == FREE)
                {
                    for (auto move : st.moves)
                    {
                        /*
                         * Calculating the indexes of a move; it checks
                         * the previous and the second previous indexes for a correct move.
                         */
                        auto first = i + move.first;
                        auto second = j + move.second;
                        auto prevFirst = move.first == 0 ? 0 : move.first < 0 ? 1
                                                                              : -1;
                        auto prevSecond = move.second == 0 ? 0 : move.second < 0 ? 1
                                                                                 : -1;
                        prevFirst += first;
                        prevSecond += second;
                        if (first >= 0 && first < tempBoard.size() &&
                            second >= 0 && second < tempBoard[i].size() &&
                            tempBoard[first][second] == PEG &&
                            tempBoard[prevFirst][prevSecond] == PEG)
                        {
                            // If all conditions are true, a new board will be constructed:
                            tempBoard[prevFirst][prevSecond] = FREE;
                            tempBoard[i][j] = tempBoard[first][second];
                            tempBoard[first][second] = FREE;
                            auto node = make_shared<Node>(tempBoard);
                            tempBoard = board->board;
                            node->parent = board;
                            node->score = board->score + 1;
                            node->lastDeletedPegIndex = make_pair(prevFirst, prevSecond);
                            nodes.push_back(node);
                        }
                    }
                }
            }
        }
        return nodes;
    }

    /*
     * This is the main loop to solve the problem.
     * All of the 5 methods use the following function:
     */
    void solve(Frontier &frontier, std::chrono::steady_clock::time_point end,
               shared_ptr<NodeSelection> selection, int maxDepth = 0)
    {
        // Pushing the first state of the board to the frontier:
        frontier.push(make_shared<Node>(st.board));

        while (frontier.empty() == false)
        {
            auto currBoard = move(frontier.pop_return());

            checkStopCriterion(frontier, currBoard, end, maxDepth);
            if (stop == true || isDepthLimitReached == true)
                return;

            saveProblemStateInfo(frontier, currBoard);

            auto nodes = searchMoves(currBoard);
            nodes = selection->select(nodes);
            for (auto &node : nodes)
            {
                frontier.push(move(node));
            }
        }
    }

    void checkStopCriterion(Frontier &frontier, shared_ptr<Node> node,
                            std::chrono::steady_clock::time_point end, int maxDepth)
    {
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - end).count();
        if (elapsedTime > 0)
            stop = true;

        // Frontier size: 3231848 frontier nodes equals to approximately 2GB.
        if (frontier.size() > 3231848)
        {
            stop = true;
            maxScore = -1;
        }

        if (maxDepth != 0 && node->score > maxDepth)
            isDepthLimitReached = true;
    }

    void saveProblemStateInfo(Frontier &frontier, shared_ptr<Node> node)
    {
        numberOfExpandedNodes++;
        maxNumberOfStoredNodes = max(static_cast<int>(frontier.size()), maxNumberOfStoredNodes);
        checkBestNode(node);
    }

    void checkBestNode(shared_ptr<Node> &node)
    {
        if (node->board[3][3] == PEG && 33 - node->score == 1)
        {
            maxScore = node->score;
            stop = true;
            bestNode = node;
        }
        else if (node->score > maxScore)
        {
            maxScore = node->score;
            bestNode = node;
        }
    }

    void printResults(long runtime)
    {
        if (maxScore == 0 || bestNode == nullptr)
        {
            cout << "No solution found - Time Limit" << endl;
        }
        else if (maxScore == -1)
        {
            cout << "No solution found - Out of Memory" << endl;
            printStates(bestNode);
        }
        else if (bestNode->board[3][3] == PEG && 33 - maxScore == 1)
        {
            cout << "Optimum solution found." << endl;
            printStates(bestNode);
        }
        else if (bestNode->board[3][3] == FREE || 33 - maxScore != 1)
        {
            cout << "Sub-optimum Solution Found with " << 33 - maxScore << " remaining pegs." << endl;
            printStates(bestNode);
        }
        std::cout << "Runtime: " << (double)runtime / 60 << " minutes." << std::endl;
        std::cout << "Number of expanded nodes: " << numberOfExpandedNodes << endl;
        std::cout << "Max number of nodes stored in the memory: " << maxNumberOfStoredNodes << endl;
        cout << endl;
    }

    void printStates(shared_ptr<Node> &node)
    {
        cout << "Board states:" << endl;
        stack<vector<vector<int>>> states;
        while (node != nullptr)
        {
            states.push(node->board);
            node = node->parent;
        }
        int counter = 0;
        while (states.empty() == false)
        {
            cout << counter++ << ". State: " << endl;
            printBoard(states.top());
            states.pop();
        }
    }

    void printBoard(vector<vector<int>> &board)
    {
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                string c;
                if (board[i][j] == FREE)
                {
                    c = '.';
                }
                else if (board[i][j] == WALL)
                {
                    c = ' ';
                }
                else
                {
                    c = 'O';
                }
                cout << c;
            }
            cout << endl;
        }
        cout << endl;
    }

    virtual void run(std::chrono::steady_clock::time_point timeLimit) = 0;
};

class Solve_DFS : public Solver
{
public:
    Solve_DFS(int timeLimit) : Solver(timeLimit)
    {
        std::cout << "Search Method: Depth-First Search, Time Limit: "
                  << timeLimit << " minutes." << endl;
    }

    void run(std::chrono::steady_clock::time_point end)
    {
        Stack s;
        shared_ptr<NodeSelection> selection = make_shared<IndexFirst>();
        solve(s, end, selection);
    }
};

class Solve_BFS : public Solver
{
public:
    Solve_BFS(int timeLimit) : Solver(timeLimit)
    {
        std::cout << "Search Method: Breadth First Search, Time Limit: "
                  << timeLimit << " minutes." << endl;
    }

    void run(std::chrono::steady_clock::time_point end)
    {
        Queue q;
        shared_ptr<NodeSelection> selection = make_shared<IndexFirst>();
        solve(q, end, selection);
    }
};

class Solve_IDS : public Solver
{
public:
    int depthLimit = 0;
    Solve_IDS(int timeLimit, int depthLimit) : Solver(timeLimit)
    {
        std::cout << "Search Method: Iterative Deepening Search, Time Limit: "
                  << timeLimit << " minutes." << endl;
        this->depthLimit = depthLimit;
    }

    void cleanStack(Stack s)
    {
        while (s.empty() == false)
        {
            s.pop_return();
        }
    }

    void run(std::chrono::steady_clock::time_point end)
    {
        shared_ptr<NodeSelection> selection = make_shared<IndexFirst>();
        for (int i = 1; i <= depthLimit; i++)
        {
            isDepthLimitReached = false;
            Stack s;
            solve(s, end, selection, i);
            if (stop == true)
                return;
            cleanStack(s);
        }
    }
};

class Solve_DFSR : public Solver
{
public:
    Solve_DFSR(int timeLimit) : Solver(timeLimit)
    {
        std::cout << "Search Method: Depth-First Search with Random Selection, Time Limit: "
                  << timeLimit << " minutes." << endl;
    }

    void run(std::chrono::steady_clock::time_point end)
    {
        shared_ptr<NodeSelection> selection = make_shared<Random>();
        Stack s;
        solve(s, end, selection);
    }
};

class Solve_DFSH : public Solver
{
public:
    Solve_DFSH(int timeLimit) : Solver(timeLimit)
    {
        std::cout << "Search Method: Depth-First Search with a Node Selection Heuristic, Time Limit: "
                  << timeLimit << " minutes." << endl;
    }

    void run(std::chrono::steady_clock::time_point end)
    {
        shared_ptr<NodeSelection> selection = make_shared<Heuristic>();
        Stack s;
        solve(s, end, selection);
    }
};

/*
 * Solve class provides time measurement and function
 * calls to the solver classes.
 */
class Solve
{
public:
    void start(Solver &s)
    {
        auto start = std::chrono::steady_clock::now();
        auto begin = start + std::chrono::minutes(s.timeLimit);
        s.run(begin);
        auto end = std::chrono::steady_clock::now();
        auto runtime = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        s.printResults(runtime);
    }
};
/*
 * Running each method in an order:
 */
int main(int argc, char **argv)
{
    Solve s;
    Solve_DFS dfs(60);
    s.start(dfs);
    Solve_BFS bfs(60);
    s.start(bfs);
    Solve_IDS ids(60, 33);
    s.start(ids);
    Solve_DFSR dfsr(60);
    s.start(dfsr);
    Solve_DFSH dfsh(60);
    s.start(dfsh);
};
