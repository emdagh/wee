#include <iostream>
#include <tuple>
#include <variant>


template <typename... States>
class state_machine
{
private:
    // the tuple m_states holds all states we'll define
    std::tuple<States...> m_states;
    // in this variant we hold a reference to the current state, it's initialized by the state at index 0
    std::variant<States*...> m_current_state{ &std::get<0>(m_states) };

public:

    // we can change a state by calling set state with a state type
    template <typename State>
    void set_state() {
        m_current_state = &std::get<State>(m_states);
    }

    // we can define certain events which call a dedicated state transition
    template <typename Event>
    void on_state_transition(const Event& event)
    {
        auto execute_on_call = [this, &event] (auto current_state) {
            current_state->on_state_transition(event).execute(*this);
        };
        // std::visit "visits" the current_state and calls the lambda with the current state
        // this means every possible state needs to implement the execute function inside the lambda
        std::visit(execute_on_call, m_current_state);
    }

    // we call on_update of each state also with std::visit like above
    void on_update()
    {
        auto execute_on_call = [this] (auto current_state) {
            current_state->on_update();
        };
        std::visit(execute_on_call, m_current_state);
    }
};



// the state transition type, where the template represents the target state
template <typename State>
struct state_transition_to
{
    // on execute we're setting the target state in our statemachine by calling execute()
    template <typename Statemachine>
    void execute(Statemachine& statemachine) {
        statemachine.template set_state<State>();
    }
};

// an invalid state transition which has an emptye execute() function
// we need this (guess what) for all transitions we wont support
struct invalid_state_transition 
{
    template <typename Machine>
    void execute(Machine&) { }
};


// specific state transition types we support
struct transition_to_run{};
struct transition_to_idle{};
struct any_other_transition{};




// state definitions
struct idle;
struct run;
struct any_state;


// idle state which implements the methods to call by the statemachine
struct idle
{

    // regular on update call 
    void on_update() const {
        std::cout << "still waiting \n";
    }

    // specific transition to run, where we return the concrete state transition to run
    // to distinguish different state transitions, we use an empty function argument here
    state_transition_to<run> on_state_transition(const transition_to_run&) const{
        std::cout << "leaving idle with transition to run \n";
        return {};
    }

    // a template function to indicate all non supported state transitions.
    template<typename Transition>
    invalid_state_transition on_state_transition(const Transition&) const {
        std::cout << "state transition: " <<  typeid(Transition).name() << " is not supported in idle! \n";
        return {};
    }
};


// the same for our run state
struct run
{
    void on_update() const {
        std::cout << "we are running! \n";
    }

    state_transition_to<idle> on_state_transition(const transition_to_idle&) const {
        std::cout << "leaving run with transition to idle \n";
        return {};
    }

    template<typename Transition>
    invalid_state_transition on_state_transition(const Transition&) const {
        std::cout << "state transition: " <<  typeid(Transition).name() << " is not supported in run! \n";
        return {};
    }
};

struct any_state
{
    // updating any state here 
    void on_update() const {
    }

    // transition to idle
    state_transition_to<idle> on_state_transition(const transition_to_idle&) const {
        return {};
    }

    // invalid transition to run (just as an example, this would be invalid)
    invalid_state_transition on_state_transition(const transition_to_run&) const{
        return {};
    }

    // invalid transition to itself
    invalid_state_transition on_state_transition(const any_other_transition&) const{
        return {};
    }

    // and a lot of other states would follow....
};



// the alias for our state machine with state idle and run
// the statemachin is initialized with idle
using example_state_machine = state_machine<idle, run>;

int main()
{
    example_state_machine machine;

    // update a couple of times in idle
    machine.on_update();
    machine.on_update();
    machine.on_update();

    
    // something happende, we change state to run
    machine.on_state_transition(transition_to_run{});

    // update a couple of times in run 
    machine.on_update();
    machine.on_update();
    machine.on_update();

    // just for demonstration here
    // call a invalid state transition
    machine.on_state_transition(transition_to_run{});
    machine.on_state_transition(transition_to_run{});

    // just a demonstration with any other state
    machine.on_state_transition(any_other_transition{});

    return 0;
}
