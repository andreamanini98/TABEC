#ifndef TOPARSER_TATILEBUFFER_H
#define TOPARSER_TATILEBUFFER_H

#include "nlohmann/json.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "defines/TOjsonAttributes.h"
#include "defines/UPPAALxmlAttributes.h"

using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;
namespace fs = std::filesystem;

#define INITIAL_MASK (1 << 0)
#define ACCEPTING_MASK (1 << 1)
#define INPUT_MASK (1 << 2)
#define OUTPUT_MASK (1 << 3)
#define LEFT_BRACE "{"
#define RIGHT_BRACE "}"
#define UNDEFINED_ID -1
#define DEFAULT_CLOCK "xy"
#define DEBUG_PRINT(fmt, ...) printf("[%s:%d] %s: " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

// a new-delete version of strdup
static char *strdup_new(const char *s)
{
    size_t len = strlen(s) + 1;
    char *new_s = new char[len];
    memcpy(new_s, s, len);
    return new_s;
}

enum class ValueType
{
    NDEFINED = 0,
    INT,
    DOUBLE,
    PARAM
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

static string COP2Symbol(COP op)
{
    switch (op)
    {
    case COP::EQ:
        return "==";
    case COP::NE:
        return "!=";
    case COP::GT:
        return ">";
    case COP::GE:
        return ">=";
    case COP::LT:
        return "<";
    case COP::LE:
        return "<=";
    case COP::ASSIGNMENT:
        return "=";
    default:
        assert(false);
    }
}

class Value
{
public:
    Value() : type_(ValueType::NDEFINED), value_(nullptr)
    {
    }

    Value(ValueType type, void *value)
    {
        type_ = type;

        if (type_ == ValueType::INT)
        {
            value_ = new int;
            *static_cast<int *>(value_) = *static_cast<int *>(value);
        }
        else if (type_ == ValueType::DOUBLE)
        {
            value_ = new double;
            *static_cast<double *>(value_) = *static_cast<double *>(value);
        }
        else if (type_ == ValueType::PARAM)
        {
            value_ = strdup_new(static_cast<char *>(value));
        }
        else if (type_ == ValueType::NDEFINED)
        {
            value_ = nullptr;
        }
    }

    void destroy()
    {
        if (type_ == ValueType::PARAM)
        {
            delete[] static_cast<char *>(value_);
        }
        else if (type_ == ValueType::INT)
        {
            delete static_cast<int *>(value_);
        }
        else if (type_ == ValueType::DOUBLE)
        {
            delete static_cast<double *>(value_);
        }
        type_ = ValueType::NDEFINED;
        value_ = nullptr;
    }

    Value(const Value &other)
    {
        type_ = other.type_;

        if (type_ == ValueType::NDEFINED)
        {
            value_ = nullptr;
        }
        else if (type_ == ValueType::PARAM)
        {
            value_ = strdup_new(static_cast<char *>(other.value_));
        }
        else if (type_ == ValueType::INT)
        {
            value_ = new int;
            *static_cast<int *>(value_) = *static_cast<int *>(other.value_);
        }
        else if (type_ == ValueType::DOUBLE)
        {
            value_ = new double;
            *static_cast<double *>(value_) = *static_cast<double *>(other.value_);
        }
    }

    Value &operator=(const Value &other)
    {
        if (this != &other)
        {
            // delete
            if (type_ == ValueType::PARAM)
            {
                delete[] static_cast<char *>(value_);
            }
            else if (type_ == ValueType::INT)
            {
                delete static_cast<int *>(value_);
            }
            else if (type_ == ValueType::DOUBLE)
            {
                delete static_cast<double *>(value_);
            }

            // copy
            type_ = other.type_;
            if (type_ == ValueType::NDEFINED)
            {
                value_ = nullptr;
            }
            else if (type_ == ValueType::PARAM)
            {
                value_ = strdup_new(static_cast<char *>(other.value_));
            }
            else if (type_ == ValueType::INT)
            {
                value_ = new int;
                *static_cast<int *>(value_) = *static_cast<int *>(other.value_);
            }
            else if (type_ == ValueType::DOUBLE)
            {
                value_ = new double;
                *static_cast<double *>(value_) = *static_cast<double *>(other.value_);
            }
        }

        return *this;
    }

    void setType(ValueType type)
    {
        type_ = type;
    }

    ValueType getType() const
    {
        return type_;
    }

    void setValue(void *value)
    {
        if (type_ == ValueType::NDEFINED)
        {
            if (value != nullptr)
            {
                assert(false);
            }
        }
        else if (type_ == ValueType::PARAM)
        {
            delete[] static_cast<char *>(value_);
            value_ = strdup_new(static_cast<char *>(value));
        }
        else if (type_ == ValueType::INT)
        {
            delete static_cast<int *>(value_);
            value_ = new int;
            *static_cast<int *>(value_) = *static_cast<int *>(value);
        }
        else if (type_ == ValueType::DOUBLE)
        {
            delete static_cast<double *>(value_);
            value_ = new double;
            *static_cast<double *>(value_) = *static_cast<double *>(value);
        }
    }

    void *getValue() const
    {
        return value_;
    }

private:
    ValueType type_;
    void *value_;
};

class Param
{
public:
    Param() : id_(UNDEFINED_ID), name_(nullptr)
    {
    }

    Param(const char *name) : id_(UNDEFINED_ID)
    {
        name_ = strdup_new(name);
    }

    Param(int id, const char *name)
    {
        id_ = id;
        name_ = strdup_new(name);
    }

    void destroy()
    {
        id_ = UNDEFINED_ID;
        delete[] name_;
        name_ = nullptr;
    }

    Param(const Param &other)
    {
        id_ = other.id_;
        name_ = strdup_new(other.name_);
    }

    Param &operator=(const Param &other)
    {
        if (this != &other)
        {
            delete[] name_;
            id_ = other.id_;
            name_ = strdup_new(other.name_);
        }
        return *this;
    }

    void setName(const char *name)
    {
        delete[] name_;
        name_ = strdup_new(name);
    }

    const char *getName() const
    {
        return name_;
    }

    void setId(int id)
    {
        id_ = id;
    }

    int getId() const
    {
        return id_;
    }

private:
    int id_;
    char *name_;
};

class BoundItem
{
public:
    BoundItem() : value_(0), is_inf_(false), is_nan_(false) {};

    void setValue(int value)
    {
        value_ = value;
    }

    void setInf(bool is_inf)
    {
        is_inf_ = is_inf;
    }

    void setNan(bool is_nan)
    {
        is_nan_ = is_nan;
    }

    int getValue() const
    {
        return value_;
    }

    bool getIsInf() const
    {
        return is_inf_;
    }

    bool getIsNan() const
    {
        return is_nan_;
    }

private:
    bool is_inf_;
    bool is_nan_;
    int value_;
};

class BoundsItem
{
public:
    BoundsItem() = default;

    void setLBound(BoundItem item)
    {
        l_bound_.setInf(item.getIsInf());
        l_bound_.setNan(item.getIsNan());
        l_bound_.setValue(item.getValue());
    }

    void setRBound(BoundItem item)
    {
        r_bound_.setInf(item.getIsInf());
        r_bound_.setNan(item.getIsNan());
        r_bound_.setValue(item.getValue());
    }

    BoundItem getLBound() const
    {
        return l_bound_;
    }

    BoundItem getRBound() const
    {
        return r_bound_;
    }

private:
    BoundItem l_bound_, r_bound_;
};

class Clock
{
public:
    Clock() : id_(UNDEFINED_ID), name_(nullptr)
    {
    }

    Clock(int id, const char *name)
    {
        id_ = id;
        name_ = strdup_new(name);
    }

    Clock(const char *name) : id_(UNDEFINED_ID)
    {
        name_ = strdup_new(name);
    }

    void destroy()
    {
        id_ = UNDEFINED_ID;
        delete[] name_;
        name_ = nullptr;
    }

    Clock(const Clock &other)
    {
        id_ = other.id_;
        name_ = strdup_new(other.name_);
    }

    Clock &operator=(const Clock &other)
    {
        if (this != &other)
        {
            delete[] name_;
            id_ = other.id_;
            name_ = strdup_new(other.name_);
        }
        return *this;
    }

    int getId() const
    {
        return id_;
    }

    void setId(int id)
    {
        id_ = id;
    }

    const char *getName() const
    {
        return name_;
    }

    void setName(const char *name)
    {
        delete[] name_;
        name_ = strdup_new(name);
    }

private:
    int id_;
    char *name_;
};

class ClockAssignment
{
public:
    ClockAssignment(const char *clock_name, ValueType type, void *value)
    {
        clock_.setId(UNDEFINED_ID);
        clock_.setName(clock_name);
        value_.setType(type);
        value_.setValue(value);
    }

    ClockAssignment(const char *clock_name, Value *right_value)
    {
        clock_.setId(UNDEFINED_ID);
        clock_.setName(clock_name);
        value_ = *right_value;
    }

    ClockAssignment(const ClockAssignment &other)
    {
        clock_ = other.clock_;
        value_ = other.value_;
    }

    void destroy()
    {
        clock_.destroy();
        value_.destroy();
    }

    ClockAssignment &operator=(const ClockAssignment &other)
    {
        if (this != &other)
        {
            clock_ = other.clock_;
            value_ = other.value_;
        }
        return *this;
    }

    const Clock &getClock() const
    {
        return clock_;
    }

    const Value &getValue() const
    {
        return value_;
    }

private:
    Clock clock_;
    Value value_;
};

class ClockConstraint
{
public:
    ClockConstraint(const char *clock_name, COP op, ValueType type, void *value)
    {
        clock_.setId(UNDEFINED_ID);
        clock_.setName(clock_name);
        op_ = op;

        value_.setType(type);

        if (value_.getType() == ValueType::NDEFINED)
        {
            assert(false);
        }
        else if (value_.getType() == ValueType::PARAM or value_.getType() == ValueType::INT or
                 value_.getType() == ValueType::DOUBLE)
        {
            value_.setValue(value);
        }
    }

    ClockConstraint(const char *clock_name, COP op, Value *value)
    {
        clock_.setId(UNDEFINED_ID);
        clock_.setName(clock_name);
        op_ = op;
        value_ = *value;
    }

    ClockConstraint(const ClockConstraint &other)
    {
        clock_ = other.clock_;
        op_ = other.op_;
        value_ = other.value_;
    }

    void destroy()
    {
        clock_.destroy();
        op_ = COP::NDEFINED;
        value_.destroy();
    }

    ClockConstraint &operator=(const ClockConstraint &other)
    {
        if (this != &other)
        {
            clock_ = other.clock_;
            op_ = other.op_;
            value_ = other.value_;
        }
        return *this;
    }

    const Clock &getClock() const
    {
        return clock_;
    }

    const COP getCOP() const
    {
        return op_;
    }

    const Value &getValue() const
    {
        return value_;
    }

private:
    Clock clock_;
    COP op_;
    Value value_;
};

class State
{
public:
    State() : id_(-1), attrs_(0)
    {
    }

    State(int id) : id_(id), attrs_(0)
    {
    }

    void setId(int id)
    {
        id_ = id;
    }

    void setInitial(bool is_initial)
    {
        if (is_initial)
            attrs_ |= (INITIAL_MASK);
        else
            attrs_ &= (~INITIAL_MASK);
    }

    void setAccepting(bool is_accepting)
    {
        if (is_accepting)
            attrs_ |= (ACCEPTING_MASK);
        else
            attrs_ &= (~ACCEPTING_MASK);
    }

    void setInput(bool is_input)
    {
        if (is_input)
            attrs_ |= (INPUT_MASK);
        else
            attrs_ &= (~INPUT_MASK);
    }

    void setOutput(bool is_output)
    {
        if (is_output)
            attrs_ |= (OUTPUT_MASK);
        else
            attrs_ &= (~OUTPUT_MASK);
    }

    int getId() const
    {
        return id_;
    }

    bool isInitial() const
    {
        return attrs_ & INITIAL_MASK;
    }

    bool isAccepting() const
    {
        return attrs_ & ACCEPTING_MASK;
    }

    bool isInput() const
    {
        return attrs_ & INPUT_MASK;
    }

    bool isOutput() const
    {
        return attrs_ & OUTPUT_MASK;
    }

    int getAttrs() const
    {
        return attrs_;
    }

private:
    int id_;
    int attrs_;
};

class Transition
{
public:
    Transition(int from, int to) : from_(from), to_(to)
    {
    }

    Transition(const Transition &other)
    {
        from_ = other.from_;
        to_ = other.to_;
        actions_ = other.actions_;
        guards_ = other.guards_;
    }

    void destroy()
    {
        for (auto action : actions_)
        {
            action->destroy();
        }

        for (auto guard : guards_)
        {
            guard->destroy();
        }
    }

    void setFrom(int from)
    {
        from_ = from;
    }

    const int getFrom() const
    {
        return from_;
    }

    void setTo(int to)
    {
        to_ = to;
    }

    const int getTo() const
    {
        return to_;
    }

    void addAction(ClockAssignment *action)
    {
        actions_.push_back(action);
    }

    const vector<ClockAssignment *> &getActions()
    {
        return actions_;
    }

    void addGuard(ClockConstraint *guard)
    {
        guards_.push_back(guard);
    }

    const vector<ClockConstraint *> &getGuards()
    {
        return guards_;
    }

private:
    int from_;
    int to_;
    vector<ClockConstraint *> guards_;
    vector<ClockAssignment *> actions_;
};

class InputComment
{
public:
    InputComment(int id)
    {
        id_ = id;
        assignment_ = nullptr;
    }

    InputComment(int id, ClockAssignment *assignment)
    {
        id_ = id;
        assignment_ = assignment;
    }

    ClockAssignment *getAssignment() const
    {
        return assignment_;
    }

    void setAssignment(ClockAssignment *assignment)
    {
        assignment_ = assignment;
    }

    int getId() const
    {
        return id_;
    }

    void setId(int id)
    {
        id_ = id;
    }

    void destroy()
    {
        if (assignment_ != nullptr)
            assignment_->destroy();
    }

private:
    int id_;
    ClockAssignment *assignment_;
};

class TATile
{
public:
    TATile()
    {
        name_ = nullptr;
    }

    TATile(const char *name)
    {
        name_ = strdup_new(name);
    }

    void destory()
    {
        delete[] name_;

        for (auto clock : clocks_)
        {
            clock->destroy();
        }
        clocks_.clear();

        for (auto param : params_)
        {
            param->destroy();
        }
        params_.clear();

        for (auto transition : transitions_)
        {
            transition->destroy();
        }
        transitions_.clear();

        for (auto bounds : boundss_)
        {
            delete bounds;
        }
        boundss_.clear();
    }

    void appendClock(Clock *clock)
    {
        clock->setId(clocks_.size());
        clocks_.push_back(clock);
    }

    void appendClock(const char *clock_name)
    {
        Clock *clock = new Clock(clocks_.size(), clock_name);
        clocks_.push_back(clock);
    }

    const vector<Clock *> &getClocks() const
    {
        return clocks_;
    }

    void appendParam(Param *param)
    {
        param->setId(params_.size());
        params_.push_back(param);
    }

    void appendParam(const char *param_name)
    {
        Param *param_new = new Param(params_.size(), param_name);
        params_.push_back(param_new);
    }

    const vector<Param *> &getParams() const
    {
        return params_;
    }

    void setName(const char *name)
    {
        assert(name_ == nullptr);
        name_ = strdup_new(name);
    }

    const char *getName() const
    {
        return name_;
    }

    int getStatesNum() const
    {
        return states_.size();
    }

    void addState(State *s)
    {
        states_.push_back(s);
    }

    const vector<State *> &getStates()
    {
        return states_;
    }

    void appendTransition(Transition *t)
    {
        transitions_.push_back(t);
    }

    const vector<Transition *> &getTransitions() const
    {
        return transitions_;
    }

    void addInputComment(InputComment *input_comment)
    {
        input_comments_.push_back(input_comment);
    }

    const vector<InputComment *> &getInputComments() const
    {
        return input_comments_;
    }

    bool setStateInitial(int state_id, bool is_initial)
    {
        if (state_id < 0 or state_id >= states_.size())
            return false;

        states_[state_id]->setInitial(is_initial);
        return true;
    }

    bool setStateAccepting(int state_id, bool is_accepting)
    {
        if (state_id < 0 or state_id >= states_.size())
            return false;

        states_[state_id]->setAccepting(is_accepting);
        return true;
    }

    bool setStateInput(int state_id, bool is_input)
    {
        if (state_id < 0 or state_id >= states_.size())
            return false;

        states_[state_id]->setInput(is_input);
        return true;
    }

    bool setStateOutput(int state_id, bool is_output)
    {
        if (state_id < 0 or state_id >= states_.size())
            return false;

        states_[state_id]->setOutput(is_output);
        return true;
    }

    void importFrom()
    {
    }

    void exportTo(string dir_path)
    {
        // double check
        if (!check())
        {
            std::cout << "TATile is not valid, cannot export to file" << std::endl;
            return;
        }

        // export to file
        if (!fs::exists(dir_path))
        {
            std::cout << "given directory path does not exist" << std::endl;
            return;
        }
        else if (!fs::is_directory(dir_path))
        {
            std::cout << "given path is not a directory" << std::endl;
            return;
        }

        fs::path file_path = dir_path;
        string file_name = string(name_) + ".tot";
        file_path.append(file_name);

        std::ofstream out_file(file_path);
        assert(out_file.is_open());

        // json write
        nlohmann::ordered_json result;
        result[TOTNAME] = name_;

        if (states_.size() == 0)
        {
            std::cout << "no states in the TATile" << std::endl;
            return;
        }

        if (boundss_.size() != 0)
        {
            result[TOBOUNDS] = nlohmann::json::array();
            for (auto bounds : boundss_)
            {
                BoundItem left = bounds->getLBound(), right = bounds->getRBound();
                nlohmann::ordered_json jobject;

                if (left.getIsInf())
                {
                    jobject[TOLEFT] = TOINF;
                }
                else if (left.getIsNan())
                {
                    jobject[TOLEFT] = TONAN;
                }
                else
                {
                    jobject[TOLEFT] = left.getValue();
                }

                if (right.getIsInf())
                {
                    jobject[TORIGHT] = TOINF;
                }
                else if (right.getIsNan())
                {
                    jobject[TORIGHT] = TONAN;
                }
                else
                {
                    jobject[TORIGHT] = right.getValue();
                }

                result[TOBOUNDS].push_back(jobject);
            }
        }

        result[TONSTATES] = states_.size();

        vector<int> ins, outs, accepts, inits;
        for (int i = 0; i < states_.size(); i++)
        {
            auto state = getStates().at(i);
            if (state->isInput())
                ins.push_back(i);
            if (state->isOutput())
                outs.push_back(i);
            if (state->isAccepting())
                accepts.push_back(i);
            if (state->isInitial())
                inits.push_back(i);
        }

        result[TOINPUT] = nlohmann::json::array();
        for (int i = 0; i < ins.size(); i++)
        {
            result[TOINPUT].push_back(ins[i]);
        }

        result[TOOUTPUT] = nlohmann::json::array();
        for (int i = 0; i < outs.size(); i++)
        {
            result[TOOUTPUT].push_back(outs[i]);
        }

        result[TOINITIAL] = nlohmann::json::array();
        for (int i = 0; i < inits.size(); i++)
        {
            result[TOINITIAL].push_back(inits[i]);
        }

        result[TOACCEPTING] = nlohmann::json::array();
        for (int i = 0; i < accepts.size(); i++)
        {
            result[TOACCEPTING].push_back(accepts[i]);
        }

        result[TONCLOCKS] = clocks_.size();
        result[TOCLOCKS] = nlohmann::json::array();
        for (int i = 0; i < clocks_.size(); i++)
        {
            nlohmann::ordered_json jclock;
            jclock[TOID] = clocks_[i]->getId();
            jclock[TONAME] = clocks_[i]->getName();
            result[TOCLOCKS].push_back(jclock);
        }

        result[TONPARAMS] = params_.size();
        result[TOPARAMS] = nlohmann::json::array();
        for (int i = 0; i < params_.size(); i++)
        {
            nlohmann::ordered_json jparam;
            jparam[TOID] = params_[i]->getId();
            jparam[TONAME] = params_[i]->getName();
            result[TOPARAMS].push_back(jparam);
        }

        result[TONTRANSITIONS] = transitions_.size();
        result[TOTRANSITIONS] = nlohmann::json::array();
        for (int i = 0; i < transitions_.size(); i++)
        {
            nlohmann::ordered_json jtransition;
            jtransition[TOFROM] = transitions_[i]->getFrom();
            jtransition[TOTO] = transitions_[i]->getTo();
            jtransition[TONGUARDS] = transitions_[i]->getGuards().size();
            if (jtransition[TONGUARDS] > 0)
            {
                jtransition[TOGUARDS] = nlohmann::json::array();
                for (int j = 0; j < transitions_[i]->getGuards().size(); j++)
                {
                    nlohmann::ordered_json jguard;
                    jguard[TOCLOCK] = transitions_[i]->getGuards()[j]->getClock().getName();
                    jguard[TOOP] = COP2Symbol(transitions_[i]->getGuards()[j]->getCOP());
                    switch (transitions_[i]->getGuards()[j]->getValue().getType())
                    {
                    case ValueType::NDEFINED:
                        assert(false);
                    case ValueType::INT:
                        jguard[TOVALUE] = *static_cast<int *>(transitions_[i]->getGuards()[j]->getValue().getValue());
                        break;
                    case ValueType::DOUBLE:
                        jguard[TOVALUE] =
                            *static_cast<double *>(transitions_[i]->getGuards()[j]->getValue().getValue());
                        break;
                    case ValueType::PARAM:
                        jguard[TOVALUE] = static_cast<char *>(transitions_[i]->getGuards()[j]->getValue().getValue());
                        break;
                    }
                    jtransition[TOGUARDS].push_back(jguard);
                }
            }
            jtransition[TONACTIONS] = transitions_[i]->getActions().size();
            if (jtransition[TONACTIONS] > 0)
            {
                jtransition[TOACTIONS] = nlohmann::json::array();
                for (int j = 0; j < transitions_[i]->getActions().size(); j++)
                {
                    nlohmann::ordered_json jaction;
                    jaction[TOCLOCK] = transitions_[i]->getActions()[j]->getClock().getName();
                    switch (transitions_[i]->getActions()[j]->getValue().getType())
                    {
                    case ValueType::NDEFINED:
                        assert(false);
                    case ValueType::INT:
                        jaction[TOVALUE] = *static_cast<int *>(transitions_[i]->getActions()[j]->getValue().getValue());
                        break;
                    case ValueType::DOUBLE:
                        jaction[TOVALUE] =
                            *static_cast<double *>(transitions_[i]->getActions()[j]->getValue().getValue());
                        break;
                    case ValueType::PARAM:
                        jaction[TOVALUE] = static_cast<char *>(transitions_[i]->getActions()[j]->getValue().getValue());
                        break;
                    }
                    jtransition[TOACTIONS].push_back(jaction);
                }
            }
            result[TOTRANSITIONS].push_back(jtransition);
        }

        result[TONINPUTCOMMENTS] = input_comments_.size();
        result[TOINPUTCOMMENTS] = nlohmann::json::array();
        for (int i = 0; i < input_comments_.size(); i++)
        {
            nlohmann::ordered_json jinputcomment;
            jinputcomment[TOID] = input_comments_[i]->getId();
            jinputcomment[TOACTION][TOCLOCK] = input_comments_[i]->getAssignment()->getClock().getName();

            switch (input_comments_[i]->getAssignment()->getValue().getType())
            {
            case ValueType::NDEFINED:
                assert(false);
            case ValueType::INT:
                jinputcomment[TOACTION][TOVALUE] =
                    *static_cast<int *>(input_comments_[i]->getAssignment()->getValue().getValue());
                break;
            case ValueType::DOUBLE:
                jinputcomment[TOACTION][TOVALUE] =
                    *static_cast<double *>(input_comments_[i]->getAssignment()->getValue().getValue());
                break;
            case ValueType::PARAM:
                jinputcomment[TOACTION][TOVALUE] =
                    static_cast<char *>(input_comments_[i]->getAssignment()->getValue().getValue());
                break;
            }

            result[TOINPUTCOMMENTS].push_back(jinputcomment);
        }

        out_file << result.dump(4) << std::endl;
    }

    void exportToJSON(string dir_path)
    {
        // double check
        if (!check())
        {
            std::cout << "TATile is not valid, cannot export to file" << std::endl;
            return;
        }

        // export to file
        if (!fs::exists(dir_path))
        {
            std::cout << "given directory path does not exist" << std::endl;
            return;
        }
        else if (!fs::is_directory(dir_path))
        {
            std::cout << "given path is not a directory" << std::endl;
            return;
        }

        fs::path file_path = dir_path;
        string file_name = string(name_) + ".totj";
        file_path.append(file_name);

        std::ofstream out_file(file_path);
        assert(out_file.is_open());

        // json write
        nlohmann::ordered_json result;
        std::unordered_map<int, std::string> state_id_to_name;

        if (states_.size() == 0)
        {
            std::cout << "no states in the TATile" << std::endl;
            return;
        }

        formJsonBoundsD(result);
        formJsonClocksD(result);
        formJsonLocationsD(result, state_id_to_name);
        formJsonTransitionsD(result, state_id_to_name);
        formJsonOthersD(result, state_id_to_name);

        out_file << result.dump(4) << std::endl;
    }

    bool postProcess()
    {
        (void)buildMap();
        // validate guards
        for (auto transition : transitions_)
        {
            for (auto guard : transition->getGuards())
            {
                const char *clock_name = guard->getClock().getName();
                if (clocks_map_.find(clock_name) == clocks_map_.end())
                {
                    return false;
                }
                if (guard->getValue().getType() == ValueType::PARAM)
                {
                    const char *param_name = static_cast<char *>(guard->getValue().getValue());
                    if (params_map_.find(param_name) == params_map_.end())
                    {
                        return false;
                    }
                }
            }

            for (auto action : transition->getActions())
            {
                const char *clock_name = action->getClock().getName();
                if (clocks_map_.find(clock_name) == clocks_map_.end())
                {
                    return false;
                }
                if (action->getValue().getType() == ValueType::PARAM)
                {
                    const char *param_name = static_cast<char *>(action->getValue().getValue());
                    if (params_map_.find(param_name) == params_map_.end())
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool check()
    {
        if (!checkTileProperties())
            return false;

        if (!checkClocks())
            return false;

        if (!checkParams())
            return false;

        if (!checkStates())
            return false;

        if (!checkTransitions())
            return false;

        return true;
    }

    void appendBounds(BoundsItem *bounds)
    {
        boundss_.push_back(bounds);
    }

private:
    void addDefaultGuards()
    {
        for (auto transition : transitions_)
        {
            transition->addGuard(new ClockConstraint(DEFAULT_CLOCK, COP::GT, ValueType::INT, new int(0)));
        }
    }

    void addDefaultActions()
    {
        for (auto transition : transitions_)
        {
            transition->addAction(new ClockAssignment(DEFAULT_CLOCK, ValueType::INT, new int(0)));
        }
    }

    bool transitionCheck(Transition trans)
    {
        int from = trans.getFrom(), to = trans.getTo();

        if (from >= states_.size() or from < 0)
            return false;
        if (to >= states_.size() or to < 0)
            return false;

        return true;
    }

    bool checkTileProperties()
    {
        if (name_ == nullptr)
            return false;

        if (states_.size() == 0)
            return false;
        else if (clocks_.size() == 0)
            return false;

        return true;
    }

    bool checkClocks()
    {
        unordered_set<int> clock_ids;
        unordered_set<string> clock_names;

        for (int i = 0; i < clocks_.size(); i++)
        {
            Clock *clock = clocks_[i];

            if (clock->getId() < 0 or clock->getName() == nullptr)
            {
                return false;
            }

            if (clock->getId() != i)
            {
                return false;
            }

            // judge if clock_id or clock_name is duplicated
            auto return1 = clock_ids.insert(clock->getId());
            std::string clock_name = string(clock->getName());
            auto return2 = clock_names.insert(clock_name);

            if (return1.second == false or return2.second == false)
            {
                std::cout << "there is a duplicated clock name or clock id" << std::endl;
                return false;
            }
        }

        return true;
    }

    bool checkParams()
    {
        unordered_set<int> param_ids;
        unordered_set<string> param_names;

        for (int i = 0; i < params_.size(); i++)
        {
            const Param &param = *params_[i];

            if (param.getId() < 0 or param.getName() == nullptr)
                return false;

            if (param.getId() != i)
            {
                return false;
            }

            // judge if param_id or param_name is duplicated
            auto return1 = param_ids.insert(param.getId());
            string param_name = string(param.getName());
            auto return2 = param_names.insert(param_name);

            if (return1.second == false or return2.second == false)
            {
                std::cout << "there is a duplicated param name or param id" << std::endl;
                return false;
            }
        }

        return true;
    }

    bool checkStates()
    {
        unordered_set<int> state_input, state_output;

        for (int i = 0; i < states_.size(); i++)
        {
            const State &state = *states_[i];
            if (state.getId() < 0 or state.getId() >= states_.size())
            {
                // something wrong
                return false;
            }

            if (state.getId() != i)
            {
                return false;
            }
        }

        return true;
    }

    bool checkTransitions()
    {
        for (int i = 0; i < transitions_.size(); i++)
        {
            if (!transitionCheck(*transitions_[i]))
                return false;
        }

        return true;
    }

    void buildMap()
    {
        for (int i = 0; i < clocks_.size(); i++)
        {
            clocks_map_[clocks_[i]->getName()] = clocks_[i]->getId();
        }

        for (int i = 0; i < params_.size(); i++)
        {
            params_map_[params_[i]->getName()] = params_[i]->getId();
        }
    }

private:
    void formJsonBoundsD(nlohmann::ordered_json &result)
    {
        std::string bound_declaration_string;
        int bound_idx = 0;

        for (const auto &bound : boundss_)
        {
            if (bound_idx != 0)
                bound_declaration_string += "|";

            const auto &left = bound->getLBound(), &right = bound->getRBound();

            bound_declaration_string += "bound";

            if (left.getIsInf())
            {
                bound_declaration_string += ":inf";
            }
            else if (left.getIsNan())
            {
                bound_declaration_string += ":nan";
            }
            else
            {
                bound_declaration_string += ":" + std::to_string(left.getValue());
            }

            if (right.getIsInf())
            {
                bound_declaration_string += ":inf";
            }
            else if (right.getIsNan())
            {
                bound_declaration_string += ":nan";
            }
            else
            {
                bound_declaration_string += ":" + std::to_string(right.getValue());
            }

            bound_idx++;
        }

        result[NTA][DECLARATION] = bound_declaration_string;
    }

    void formJsonClocksD(nlohmann::ordered_json &result)
    {
        std::string clock_declaration_string;
        int clock_idx = 0;
        clock_declaration_string += "// Place local declarations here.\nclock ";

        for (const auto &clock : clocks_)
        {
            clock_declaration_string += (clock_idx != 0) ? ", " : "";
            clock_declaration_string += std::string(clock->getName());
            clock_idx++;
        }

        clock_declaration_string += ";";
        result[NTA][TEMPLATE][DECLARATION] = clock_declaration_string;
    }

    void formJsonLocationsD(nlohmann::ordered_json &result, std::unordered_map<int, std::string> &state_id_to_name)
    {
        nlohmann::ordered_json &locations = result[NTA][TEMPLATE][LOCATION];
        locations = nlohmann::json::array();

        for (int i = 0; i < states_.size(); i++)
        {
            nlohmann::ordered_json location;
            std::string state_name = "id" + std::to_string(i);
            location[ID] = state_name;
            state_id_to_name.emplace(i, state_name);

            if (states_[i]->isInput())
                location[NAME][TEXT] = "in";

            if (states_[i]->isOutput())
                location[NAME][TEXT] = "out";

            if (states_[i]->isAccepting())
                location[COLOR] = "SYMBOL";

            for (int j = 0; j < input_comments_.size(); j++)
            {
                if (i == input_comments_[j]->getId() and input_comments_[j]->getAssignment() != nullptr)
                {
                    std::string input_comment_string;
                    input_comment_string +=
                        std::string(input_comments_[j]->getAssignment()->getClock().getName()) + " = ";
                    switch (input_comments_[j]->getAssignment()->getValue().getType())
                    {
                    case ValueType::NDEFINED:
                        assert(false);
                    case ValueType::INT:
                        input_comment_string += std::to_string(
                            *static_cast<int *>(input_comments_[j]->getAssignment()->getValue().getValue()));
                        break;
                    case ValueType::DOUBLE:
                        input_comment_string += std::to_string(
                            *static_cast<double *>(input_comments_[j]->getAssignment()->getValue().getValue()));
                        break;
                    case ValueType::PARAM:
                        input_comment_string += std::string(
                            static_cast<char *>(input_comments_[j]->getAssignment()->getValue().getValue()));
                        break;
                    }
                    location[LABEL][TEXT] = input_comment_string;
                    location[LABEL][KIND] = "comments";
                }
            }

            locations.push_back(location);
        }
    }

    void formJsonTransitionsD(nlohmann::ordered_json &result, std::unordered_map<int, std::string> &state_id_to_name)
    {
        nlohmann::ordered_json &transitions = result[NTA][TEMPLATE][TRANSITION];
        transitions = nlohmann::json::array();
        int transitions_num = transitions_.size();
        int transition_idx = state_id_to_name.size();

        if (transitions_num != 0)
        {
            for (const auto &transition : transitions_)
            {
                nlohmann::ordered_json jtransition;

                jtransition[ID] = "id" + std::to_string(transition_idx);
                jtransition[SOURCE][REF] = state_id_to_name.at(transition->getFrom());
                jtransition[TARGET][REF] = state_id_to_name.at(transition->getTo());

                if (transition->getGuards().size() != 0 or transition->getActions().size() != 0)
                    jtransition[LABEL] = nlohmann::json::array();

                if (transition->getGuards().size() != 0)
                {
                    std::string guards_string;
                    int guard_idx = 0;

                    for (const auto &guard : transition->getGuards())
                    {
                        if (guard_idx != 0)
                            guards_string += " && ";

                        guards_string += std::string(guard->getClock().getName());
                        guards_string += " ";
                        guards_string += COP2Symbol(guard->getCOP());
                        guards_string += " ";

                        if (guard->getValue().getType() == ValueType::INT)
                        {
                            guards_string += std::to_string(*static_cast<int *>(guard->getValue().getValue()));
                        }
                        else if (guard->getValue().getType() == ValueType::DOUBLE)
                        {
                            guards_string += std::to_string(*static_cast<double *>(guard->getValue().getValue()));
                        }
                        else if (guard->getValue().getType() == ValueType::PARAM)
                        {
                            guards_string += std::string(static_cast<char *>(guard->getValue().getValue()));
                        }
                        else
                        {
                            std::cerr << "guard value type is not supported" << std::endl;
                            assert(false);
                        }

                        guard_idx++;
                    }

                    nlohmann::ordered_json jguards;
                    jguards[TEXT] = guards_string;
                    jguards[KIND] = "guard";
                    jtransition[LABEL].push_back(jguards);
                }

                if (transition->getActions().size() != 0)
                {
                    std::string actions_string;
                    int action_idx = 0;

                    for (const auto &action : transition->getActions())
                    {
                        if (action_idx != 0)
                            actions_string += " && ";

                        actions_string += std::string(action->getClock().getName());
                        actions_string += " = ";

                        if (action->getValue().getType() == ValueType::INT)
                        {
                            actions_string += std::to_string(*static_cast<int *>(action->getValue().getValue()));
                        }
                        else if (action->getValue().getType() == ValueType::DOUBLE)
                        {
                            actions_string += std::to_string(*static_cast<double *>(action->getValue().getValue()));
                        }
                        else if (action->getValue().getType() == ValueType::PARAM)
                        {
                            actions_string += std::string(static_cast<char *>(action->getValue().getValue()));
                        }
                        else
                        {
                            std::cerr << "action value type is not supported" << std::endl;
                            assert(false);
                        }

                        action_idx++;
                    }

                    nlohmann::ordered_json jactions;
                    jactions[TEXT] = actions_string;
                    jactions[KIND] = "assignment";
                    jtransition[LABEL].push_back(jactions);
                }

                transitions.push_back(jtransition);
            }
        }
    }

    void formJsonOthersD(nlohmann::ordered_json &result, const std::unordered_map<int, std::string> &state_id_to_name)
    {
        int initial_state_id = -1;

        for (int i = 0; i < states_.size(); i++)
        {
            if (states_[i]->isInitial())
            {
                initial_state_id = i;
                break;
            }
        }

        if (initial_state_id != -1)
        {
            result[NTA][TEMPLATE][INIT][REF] = state_id_to_name.at(initial_state_id);
        }
    }

private:
    char *name_;
    vector<BoundsItem *> boundss_;
    unordered_map<string, int> clocks_map_;
    unordered_map<string, int> params_map_;
    vector<Clock *> clocks_;
    vector<Param *> params_;
    vector<State *> states_;
    vector<Transition *> transitions_;
    vector<InputComment *> input_comments_;
};

class TOBuffer
{
public:
    TOBuffer(const TOBuffer &) = delete;
    TOBuffer &operator=(const TOBuffer &) = delete;

    static TOBuffer &getInstance()
    {
        static TOBuffer instance;
        return instance;
    }

    void initialize()
    {
        if (tile_ != nullptr)
            delete tile_;

        tile_ = new TATile;
    }

    void destroy()
    {
        if (tile_ != nullptr)
        {
            tile_->destory();
            tile_ = nullptr;
        }
    }

    // tile_ not private actually
    TATile *tile()
    {
        return tile_;
    }

private:
    TOBuffer() : tile_(nullptr)
    {
    }

    void addDefaultClocks()
    {
        tile_->appendClock(DEFAULT_CLOCK);
    }

private:
    TATile *tile_;
};

#endif  // TOPARSER_TATILEBUFFER_H
