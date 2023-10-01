#include "controller.h"
#include "robot/robot.h"
#include "wdLog/log.h"
#include <iostream>

Controller::~Controller()
{
}
Controller::Controller()
{
}
void Controller::setpControllerCommand(ControllerCommand *pControllerCommand)
{
    this->pControllerCommand = pControllerCommand;
}
void Controller::setpControllerState(ControllerState *pControllerState)
{
    this->pControllerState = pControllerState;
}
const ControllerCommand *Controller::getpControllerCommand()
{
    return pControllerCommand;
}
const ControllerState *Controller::getpControllerState()
{
    return pControllerState;
}
bool Controller::createRunTask(const std::vector<double> &q, TaskSpace plannerTaskSpace)
{
    if (pControllerState->controllerStatus != RunStatus::wait_)
    {
        wdlog_w("Controller", "机器人运动中，创建运行任务失败\n");
        return false;
    }

    if (pControllerCommand->runSign)
    {
        wdlog_e("Controller", "runSign信号未清，创建运行任务失败\n");
        return false;
    }

    pControllerCommand->plannerTaskSpace = plannerTaskSpace;
    for (int i = 0; i < q.size(); i++)
    {
        pControllerCommand->q_final[i] = q[i];
    }
    pControllerCommand->runSign = true;
    return true;
}
bool Controller::changeControllerLaw(ControllerLawType type)
{
    if (pControllerState->controllerStatus != RunStatus::wait_)
    {
        wdlog_w("Controller", "机器人运动中，切换控制律失败\n");
        return false;
    }
    pControllerCommand->controllerLawType_d = type;
    return true;
}
void Controller::stopRun()
{
    if (pControllerState->controllerStatus != RunStatus::run_)
    {
        wdlog_w("Controller", "机器人未运动，停止失败\n");
        return;
    }
    if (pControllerCommand->stopSign)
    {
        wdlog_e("Controller", "stopSign信号未清，创建运行任务失败\n");
        return;
    }
    pControllerCommand->stopSign = true;
}
void Controller::setRunSpeed(double runSpeedRatio)
{
    this->pControllerCommand->runSpeed_d = std::max(0.01, std::min(1.0, runSpeedRatio));
}
void Controller::setJogSpeed(double jogSpeedRatio)
{
    this->pControllerCommand->jogSpeed_d = std::max(0.01, std::min(1.0, jogSpeedRatio));
}

double Controller::getRunSpeed()
{
    return this->pControllerCommand->runSpeed_d;
}
double Controller::getJogSpeed()
{
    return this->pControllerCommand->jogSpeed_d;
}
void Controller::setLimit(Robot *robot, std::vector<double> &qMax, std::vector<double> &qMin, std::vector<double> &dqLimit,
                          std::vector<double> &ddqLimit, std::vector<double> &dddqLimit)
{
    if (pControllerCommand->newLimit)
    {
        wdlog_e("Controller", "newLimit信号未清，限位设置失败\n");
        return;
    }
    std::copy(qMax.begin(), qMax.end(), this->pControllerCommand->qMax);
    std::copy(qMin.begin(), qMin.end(), this->pControllerCommand->qMin);
    std::copy(dqLimit.begin(), dqLimit.end(), this->pControllerCommand->dqLimit);
    std::copy(ddqLimit.begin(), ddqLimit.end(), this->pControllerCommand->ddqLimit);
    std::copy(dddqLimit.begin(), dddqLimit.end(), this->pControllerCommand->dddqLimit);
    pControllerCommand->newLimit = true;
}
void Controller::getLimit(Robot *robot, std::vector<double> &qMax, std::vector<double> &qMin, std::vector<double> &dqLimit,
                          std::vector<double> &ddqLimit, std::vector<double> &dddqLimit)
{
    // for (int i = 0; i < robot->getRobotDof(); i++)
    // {
    //     qMax[i] = pControllerCommand->qMax[i];
    //     qMin[i] = pControllerCommand->qMin[i];
    //     dqLimit[i] = pControllerCommand->dqLimit[i];
    //     ddqLimit[i] = pControllerCommand->ddqLimit[i];
    //     ddqLimit[i] = pControllerCommand->dddqLimit[i];
    // }
}
