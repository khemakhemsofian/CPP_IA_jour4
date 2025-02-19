#include <iostream>
#include <vector>
#include <string>

class State {
public:
    bool hasFood = false;  // La nourriture est-elle disponible ?
    bool hasWater = false; // L'eau est-elle disponible ?
    bool isUnderAttack = false;  // L'agent est-il attaqué ?
    int hunger = 100;      // Le niveau de faim (100 = faim maximale)
    int thirst = 100;      // Le niveau de soif (100 = soif maximale)

    bool HasFood() const { return hasFood; }
    bool HasWater() const { return hasWater; }
    bool IsUnderAttack() const { return isUnderAttack; }
    int GetHunger() const { return hunger; }
    int GetThirst() const { return thirst; }

    void SetFood(bool food) { hasFood = food; }
    void SetWater(bool water) { hasWater = water; }
    void ReduceHunger() { hunger = std::max(0, hunger - 50); }  // Réduit la faim de 50 points
    void ReduceThirst() { thirst = std::max(0, thirst - 50); }  // Réduit la soif de 50 points
    void SetHunger(int level) { hunger = level; }
    void SetThirst(int level) { thirst = level; }
    void SetUnderAttack(bool attack) { isUnderAttack = attack; }
};

class Action {
public:
    virtual bool CanExecute(const State& state) = 0;
    virtual void Execute(State& state) = 0;
    virtual ~Action() {}
};

class EatAction : public Action {
public:
    bool CanExecute(const State& state) override {
        return state.HasFood() && state.GetHunger() > 0;
    }

    void Execute(State& state) override {
        std::cout << "L'agent mange.\n";
        state.ReduceHunger();  // Réduit la faim après avoir mangé
        state.SetFood(false);   // Après avoir mangé, il n'y a plus de nourriture
    }
};
class DrinkWaterAction : public Action
{
public:
    bool CanExecute(const State& state) override
    {
        return state.HasWater() && state.GetThirst() > 0;
    }
    void Execute(State& state) override
    {
        std::cout << "L'agent boit de l'eau.\n";
        state.ReduceThirst();  // Réduit la soif après avoir bu
        state.SetWater(false);   // Après avoir bu, il n'y a plus d'eau
    }
};
class FleeAction : public Action {
public:
    bool CanExecute(const State& state)override
    {
        return state.IsUnderAttack();
    }
    void Execute(State& state) override
    {
        std::cout << "L'agent fuit pour survivre !\n";
        state.SetUnderAttack(false);  // L'agent n'est plus attaqué
    }
};

class SearchFoodAction : public Action {
public:
    bool CanExecute(const State& state) override {
        return !state.HasFood();  // Peut chercher de la nourriture si l'agent n'en a pas
    }

    void Execute(State& state) override {
        std::cout << "L'agent cherche de la nourriture.\n";
        state.SetFood(true);  // Trouve de la nourriture
    }
};
class SearchWaterAction : public Action
{
public:
    bool CanExecute(const State& state) override
    {
        return !state.HasWater();  // Peut chercher de l'eau si l'agent n'en a pas
    }
    void Execute(State& state) override
    {
        std::cout << "L'agent cherche de l'eau.\n";
        state.SetWater(true);  // Trouve de l'eau
    }
};
enum class Goal {
    Manger,
    ChercherNourriture,
    Boire,
};


class GOAPPlanner {
public:
    std::vector<Action*> Plan(const State& initialState, Goal goal) {
        std::vector<Action*> plan;

        if (initialState.IsUnderAttack())
        {
            plan.push_back(new FleeAction());
        }
        else if (goal == Goal::Manger) {
            if (initialState.GetHunger() > 0 && !initialState.HasFood()) {
                plan.push_back(new SearchFoodAction());
                plan.push_back(new EatAction());
            }
            else if (initialState.HasFood()) {
                plan.push_back(new EatAction());
            }
        }
        else if (goal == Goal::Boire)
        {
            if (initialState.GetThirst() > 0 && !initialState.HasWater())
            {
                plan.push_back(new SearchWaterAction());
                plan.push_back(new DrinkWaterAction());
            }
            else if (initialState.HasWater())
            {
                plan.push_back(new DrinkWaterAction());
            }
        }
        return plan;
    }
};
class GOAPAgent {
private:
    State state;
    GOAPPlanner planner;

public:
    GOAPAgent() {
        state.SetHunger(100);  // Initialement, l'agent a faim
        state.SetThirst(100);  // Initialement, l'agent a soif
        if (rand() % 5 == 0)
        {
            state.SetUnderAttack(true);  // L'agent est attaqué
            std::cout << "Alerte: L'agent est attaqué !\n";
        }
    }

    void SetUnderAttack(bool attack) {
        state.SetUnderAttack(attack);
    }

    void PerformActions() {
        Goal goal;  // L'objectif de l'agent est de manger
        if (state.IsUnderAttack())
        {
            goal = Goal::Manger;
        }
        else if (state.GetThirst() > state.GetHunger())
        {
            goal = Goal::Boire;
        }
        else
        {
            goal = Goal::Manger;
        }

        std::vector<Action*> plan = planner.Plan(state, goal);

        for (auto action : plan) {
            if (action->CanExecute(state)) {
                action->Execute(state);  // Exécute l'action
            }
            else {
                std::cout << "Action impossible : " << typeid(*action).name() << "\n";
            }
            delete action;  // Libérer la mémoire
        }
    }

    void PrintState() {
        std::cout << "Faim: " << state.GetHunger() << "\n";
        std::cout << "Soif: " << state.GetThirst() << "\n";
        std::cout << "Nourriture disponible: " << (state.HasFood() ? "Oui" : "Non") << "\n";
        std::cout << "Eau disponible: " << (state.HasWater() ? "Oui" : "Non") << "\n";
        std::cout << "L'agent est attaqué: " << (state.IsUnderAttack() ? "Oui" : "Non") << "\n";
        if (!state.IsUnderAttack())
        {
            std::cout << "L'agent a fui avec sucess.\n";
        }
    }
};


int main() {
    GOAPAgent agent;

    std::cout << "Etat initial de l'agent:\n";
    agent.PrintState();

    // Forcer l'état d'attaque pour tester la fuite
    agent.SetUnderAttack(true);
    std::cout << "\nL'agent est maintenant attaqué !\n";
    std::cout << "\nL'agent commence ses actions...\n";
    agent.PerformActions();  // L'agent va chercher de la nourriture, puis manger

    std::cout << "\nEtat de l'agent après avoir effectué les actions:\n";
    agent.PrintState();

    return 0;
}
