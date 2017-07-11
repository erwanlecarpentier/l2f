#ifndef L2FSIM_B03_NODE_HPP_
#define L2FSIM_B03_NODE_HPP_

#include <cstdio>
#include <cstdlib>

/**
 * @file b03_node.hpp
 * @brief Node for MCTS for 'beeler_glider.hpp' model
 * @version 1.0
 * @since 1.0
 * @note compatibility: 'beeler_glider.hpp'; 'beeler_glider_state.hpp'; 'beeler_glider_command.hpp'
 */

namespace L2Fsim{

class b03_node {
public:
    /**
     * @brief Attributes
     * @param {beeler_glider_state} s; state of the node
     * @param {b03_node *} parent; pointer to the parent node
     * @param {std::vector<beeler_glider_command>} actions; available actions
     * @param {std::vector<double>} Q_values; (state,action) values
     * @param {std::vector<double>} nb_visits; (state,action) numbers of visits
     * @param {std::vector<double>} rewards; (state,action) rewards
     * @param {std::vector<b03_node>} children; (state,action) resulting children
     * @param {unsigned int} incoming_action_indice; indice of the action taken by the parent
     * @param {unsigned int} depth;
     * @warning the pointer to the parent 'parent' is obsolete if the node is root, make use of the boolean 'is_root_node'
     */
    beeler_glider_state s;
    b03_node *parent;
    std::vector<beeler_glider_command> actions;
    std::vector<double> Q_values;
    std::vector<double> rewards;
    std::vector<unsigned int> nb_visits;
    std::vector<b03_node> children;
    unsigned int incoming_action_indice;
    unsigned int depth;
    unsigned int total_nb_visits;

    /** @brief Empty constructor */
    b03_node() {}

    /** @brief Constructor */
    b03_node(
        beeler_glider_state _s,
        b03_node *_parent,
        std::vector<beeler_glider_command> _actions,
        unsigned int _incoming_action_indice,
        unsigned int _depth = 0) :
        s(_s),
        parent(_parent),
        actions(_actions),
        incoming_action_indice(_incoming_action_indice),
        depth(_depth)
    {
        unsigned int sz = actions.size();
        Q_values = std::vector<double>(sz,0.);
        rewards = std::vector<double>(sz,0.);
        nb_visits = std::vector<unsigned int>(sz,0);
        total_nb_visits = 0;
    }

    void print() {
        std::string sep = "   ";
        std::cout << "d = " << depth << " ";
        std::cout << "N = " << total_nb_visits << " ";
        std::cout << "Nc = " << nb_visits[0] << " " << nb_visits[1] << " " << nb_visits[2] << " ";
        std::cout << "Qc = " << Q_values[0] << " " << Q_values[1] << " " << Q_values[2] << sep;
        std::cout << "indincaction = " << incoming_action_indice << " ";
        std::cout << "nbchild = " << children.size() << " ";
        std::cout << "pos = " << s.x << " " << s.y << " " << s.z << sep;
        std::cout << "this = " << this << " ";
        std::cout << "parent = " << parent << "\n";
    }

    /** @brief Boolean test for termination criterion */
    bool is_terminal() {
        return s.is_out_of_bounds();
    }

    /**
     * @brief Boolean test for a node being fully expanded or not
     * @note A node is fully expanded if all of its available actions have been tried at least once and consequently the number of children is equal to the number of actions
     */
    bool is_fully_expanded() {
        return (children.size() == actions.size()) ? true : false;
        /* TRM (outdated)
        for(auto &n : nb_visits) {
            if (n == 0) {return false;}
        }
        return true;
        */
    }
};

}

#endif
