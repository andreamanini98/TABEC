#ifndef TOPARSER_TATILEBUFFER_H
#define TOPARSER_TATILEBUFFER_H

#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <cassert>

using std::string;
using std::vector;

// a new-delete version of strdup
static char *strdup_new(const char *s)
{
    size_t len = strlen(s) + 1;
    char *new_s = new char[len];
    memcpy(new_s, s, len);
    return new_s;
}

enum ValueType
{
    NDEFINED = 0,
    INT,
    FLOAT,
    PARAM
};

class Value
{
public:
    Value() : type_(ValueType::NDEFINED), param_name_(nullptr), target_value_(0) {}

    Value(ValueType type, const char *param_name, double target_value)
    {
        type_ = type;
        param_name_ = strdup_new(param_name);
        target_value_ = target_value;
    }

    ~Value()
    {
        delete[] param_name_;
    }

    Value(const Value &other)
    {
        type_ = other.type_;
        param_name_ = strdup_new(other.param_name_);
        target_value_ = other.target_value_;
    }

    Value &operator=(const Value &other)
    {
        if (this != &other)
        {
            delete[] param_name_;
            type_ = other.type_;
            param_name_ = strdup_new(other.param_name_);
            target_value_ = other.target_value_;
        }
        return *this;
    }

    void output()
    {
        switch (type_)
        {
        case ValueType::INT:
            printf("type: int, value: %d", (int)target_value_);
            break;
        case ValueType::FLOAT:
            printf("type: float, value: %f", target_value_);
            break;
        case ValueType::PARAM:
            printf("type: param, name: %s", param_name_);
            break;
        default:
            assert(false);
        }
    }

private:
    ValueType type_;
    char *param_name_;
    double target_value_;
};

enum class COP
{
    NDEFINED = 0,
    EQ,
    NE,
    GT,
    GE,
    LT,
    LE,
    ASSIGNMENT
};

class Param
{
public:
    Param()
    {
        param_id_ = -1;
        param_name_ = nullptr;
    }

    Param(int id, const char *name)
    {
        param_id_ = id;
        param_name_ = strdup_new(name);
    }

    ~Param()
    {
        delete[] param_name_;
    }

    Param(const Param &other)
    {
        param_id_ = other.param_id_;
        param_name_ = strdup_new(other.param_name_);
    }

    Param &operator=(const Param &other)
    {
        if (this != &other)
        {
            delete[] param_name_;
            param_id_ = other.param_id_;
            param_name_ = strdup_new(other.param_name_);
        }
        return *this;
    }

    void output()
    {
        printf("(%d, %s)", param_id_, param_name_);
    }

    void setParamName(const char *name)
    {
        delete[] param_name_;
        param_name_ = strdup_new(name);
    }

    const char *getParamName() const
    {
        return param_name_;
    }

    void setParamId(int id)
    {
        param_id_ = id;
    }

    int getParamId() const
    {
        return param_id_;
    }

private:
    int param_id_;
    char *param_name_;
};

class Clock
{
public:
    Clock()
    {
        clock_id_ = -1;
        clock_name_ = nullptr;
    }

    Clock(int id, const char *name)
    {
        clock_id_ = id;
        clock_name_ = strdup_new(name);
    }

    ~Clock()
    {
        delete[] clock_name_;
    }

    Clock(const Clock &other)
    {
        clock_id_ = other.clock_id_;
        clock_name_ = strdup_new(other.clock_name_);
    }

    Clock &operator=(const Clock &other)
    {
        if (this != &other)
        {
            delete[] clock_name_;
            clock_id_ = other.clock_id_;
            clock_name_ = strdup_new(other.clock_name_);
        }
        return *this;
    }

    int getClockId() const
    {
        return clock_id_;
    }

    void setClockId(int id)
    {
        clock_id_ = id;
    }

    const char *getClockName() const
    {
        return clock_name_;
    }

    void setClockName(const char *name)
    {
        delete[] clock_name_;
        clock_name_ = strdup_new(name);
    }

    void output()
    {
        printf("(%d, %s)", clock_id_, clock_name_);
    }

private:
    int clock_id_;
    char *clock_name_;
};

class ClockAssignment
{
public:
    ClockAssignment(int clock_id, const char *name, int target_value) : target_value_(target_value)
    {
        clock_.setClockId(clock_id);
        clock_.setClockName(name);
    }

    ClockAssignment(const ClockAssignment &other)
    {
        clock_ = other.clock_;
        target_value_ = other.target_value_;
    }

    ClockAssignment &operator=(const ClockAssignment &other)
    {
        if (this != &other)
        {
            clock_ = other.clock_;
            target_value_ = other.target_value_;
        }
        return *this;
    }

    void output()
    {
        printf("clock id: %d, clock name: %s, target value: %d\n", clock_.getClockId(), clock_.getClockName(), target_value_);
    }

private:
    Clock clock_;
    int target_value_;
};

class ClockConstraint
{
public:
    ClockConstraint(int clock_id, const char *name, COP op, Value right_value) : op_(op), right_value_(right_value)
    {
        clock_.setClockId(clock_id);
        clock_.setClockName(name);
        op_ = op;
        right_value_ = right_value;
    }

    ClockConstraint(const ClockConstraint &other)
    {
        clock_ = other.clock_;
        op_ = other.op_;
        right_value_ = other.right_value_;
    }

    ClockConstraint &operator=(const ClockConstraint &other)
    {
        if (this != &other)
        {
            clock_ = other.clock_;
            op_ = other.op_;
            right_value_ = other.right_value_;
        }
        return *this;
    }

    void output()
    {
        printf("clock id: %d name:%s, ", clock_.getClockId(), clock_.getClockName());
        switch (op_)
        {
        case COP::EQ:
            printf("op: =, ");
            break;
        case COP::NE:
            printf("op: !=, ");
            break;
        case COP::GT:
            printf("op: >, ");
            break;
        case COP::GE:
            printf("op: >=, ");
            break;
        case COP::LT:
            printf("op: <, ");
            break;
        case COP::LE:
            printf("op: <=, ");
            break;
        default:
            assert(false);
        }
        right_value_.output();
    }

private:
    Clock clock_;
    COP op_;
    Value right_value_;
};

class Transition
{
public:
    Transition(int from, int to) : from_(from), to_(to) {}

    Transition(const Transition &other)
    {
        from_ = other.from_;
        to_ = other.to_;
        actions_ = other.actions_;
        guards_ = other.guards_;
    }

    void addAction(ClockAssignment action)
    {
        actions_.push_back(action);
    }

    void addGuard(ClockConstraint guard)
    {
        guards_.push_back(guard);
    }

    void output()
    {
        printf("{\n");
        printf("from:%d to:%d\n", from_, to_);
        printf("%d actions:\n", (int)actions_.size());
        for (int i = 0; i < actions_.size(); i++)
        {
            actions_[i].output();
        }
        printf("%d guards:\n", (int)guards_.size());
        for (int i = 0; i < (int)guards_.size(); i++)
        {
            guards_[i].output();
        }
        printf("}\n");
    }

    void setFrom(int from)
    {
        from_ = from;
    }

    int getFrom() const
    {
        return from_;
    }

    void setTo(int to)
    {
        to_ = to;
    }

    int getTo() const
    {
        return to_;
    }

    void addAction(const ClockAssignment &action)
    {
        actions_.push_back(action);
    }

    const vector<ClockAssignment> &getActions() const
    {
        return actions_;
    }

    void addGuard(const ClockConstraint &guard)
    {
        guards_.push_back(guard);
    }

    const vector<ClockConstraint> &getGuards() const
    {
        return guards_;
    }

private:
    int from_;
    int to_;
    vector<ClockAssignment> actions_;
    vector<ClockConstraint> guards_;
};

class TATile
{
public:
    TATile()
    {
        tile_name_ = nullptr;
        locations_num_ = 0;
    }

    TATile(const char *name, int locations_num) : locations_num_(locations_num)
    {
        tile_name_ = strdup_new(name);
    }

    ~TATile()
    {
        delete[] tile_name_;
    }

    void addClock(const Clock &a)
    {
        clocks_.emplace_back(a.getClockId(), a.getClockName());
    }

    const vector<Clock> &getClocks() const
    {
        return clocks_;
    }

    void setName(const char *name)
    {
        assert(tile_name_ == nullptr);
        tile_name_ = strdup_new(name);
    }

    const char *getName() const
    {
        return tile_name_;
    }

    void setLocationsNum(int num)
    {
        assert(locations_num_ == 0);
        locations_num_ = num;
    }

    int getLocationsNum() const
    {
        return locations_num_;
    }

    void addAcceptingState(int state)
    {
        accepting_states_.push_back(state);
    }

    const vector<int> &getAcceptingStates() const
    {
        return accepting_states_;
    }

    void addInitialState(int state)
    {
        initial_states_.push_back(state);
    }

    const vector<int> &getInitialStates() const
    {
        return initial_states_;
    }

    void addInputState(int state)
    {
        input_states_.push_back(state);
    }

    const vector<int> &getInputStates() const
    {
        return input_states_;
    }

    void addOutputState(int state)
    {
        output_states_.push_back(state);
    }

    const vector<int> &getOutputStates() const
    {
        return output_states_;
    }

    void addTransition(Transition *t)
    {
        transitions_.push_back(*t);
    }

    const vector<Transition> &getTransitions() const
    {
        return transitions_;
    }

    void output()
    {
        printf("----------\n");
        printf("tile name: %s\n", tile_name_);
        printf("locations num: %d\n", locations_num_);
        printf("%d clocks:\n", (int)clocks_.size());
        for (int i = 0; i < (int)clocks_.size(); i++)
        {
            clocks_[i].output();
            if (i == (int)clocks_.size() - 1)
                printf("\n");
            else
                printf(",");
        }
        printf("accepting states:\n");
        for (int i = 0; i < (int)accepting_states_.size(); i++)
        {
            printf("%d ", accepting_states_[i]);
            if (i == (int)accepting_states_.size() - 1)
                printf("\n");
        }
        printf("initial states:\n");
        for (int i = 0; i < (int)initial_states_.size(); i++)
        {
            printf("%d ", initial_states_[i]);
            if (i == (int)initial_states_.size() - 1)
                printf("\n");
        }
        printf("input states:\n");
        for (int i = 0; i < (int)input_states_.size(); i++)
        {
            printf("%d ", input_states_[i]);
            if (i == (int)input_states_.size() - 1)
                printf("\n");
        }
        printf("output states:\n");
        for (int i = 0; i < (int)output_states_.size(); i++)
        {
            printf("%d ", output_states_[i]);
            if (i == (int)output_states_.size() - 1)
                printf("\n");
        }
        printf("%d transitions:\n", (int)transitions_.size());
        for (int i = 0; i < (int)transitions_.size(); i++)
        {
            transitions_[i].output();
        }
        printf("----------\n");
    }

    bool check()
    {
        if (tile_name_ == nullptr)
            return false;
        else
        {
            // ITODO: check if name is valid
        }

        for (int i = 0; i < accepting_states_.size(); i++)
        {
            if (accepting_states_[i] < 0 or accepting_states_[i] >= locations_num_)
                return false;
        }

        for (int i = 0; i < initial_states_.size(); i++)
        {
            if (initial_states_[i] < 0 or initial_states_[i] >= locations_num_)
                return false;
        }

        for (int i = 0; i < input_states_.size(); i++)
        {
            if (input_states_[i] < 0 or input_states_[i] >= locations_num_)
                return false;
        }

        for (int i = 0; i < output_states_.size(); i++)
        {
            if (output_states_[i] < 0 or output_states_[i] >= locations_num_)
                return false;
        }

        for (int i = 0; i < transitions_.size(); i++)
        {
            transitionCheck(transitions_[i]);
        }

        return true;
    }

private:
    bool transitionCheck(Transition trans)
    {
        if (trans.getFrom() >= locations_num_ or trans.getFrom() < 0)
            return false;
        if (trans.getTo() >= locations_num_ or trans.getTo() < 0)
            return false;
        return true;
    }

private:
    vector<Clock> clocks_;
    char *tile_name_;
    int locations_num_;
    vector<int> accepting_states_;
    vector<int> initial_states_;
    vector<int> input_states_;
    vector<int> output_states_;
    vector<Transition> transitions_;
};

/*
 * TATileBuffer is used to store all relevant variables for tile
 * In other words, it's a representation for a tile in memory
 */
class TATileBuffer
{
public:
    TATileBuffer(const TATileBuffer &) = delete;
    TATileBuffer &operator=(const TATileBuffer &) = delete;

    static TATileBuffer &getInstance()
    {
        static TATileBuffer instance;
        return instance;
    }

    void initialize()
    {
        if (tile_ == nullptr)
        {
            tile_ = new TATile;
        }

        addDefaultClocks();
    }

    void destroy()
    {
        if (tile_ != nullptr)
        {
            delete tile_;
            tile_ = nullptr;
        }
    }

    // tile_ not private actually
    TATile *tile()
    {
        return tile_;
    }

private:
    TATileBuffer() : tile_(nullptr) {}
 
    void addDefaultClocks()
    {
        tile_->addClock(Clock(0, "x"));
        tile_->addClock(Clock(1, "y"));
        tile_->addClock(Clock(2, "xy"));
    }

private:
    TATile *tile_;
};

#endif // TOPARSER_TATILEBUFFER_H
