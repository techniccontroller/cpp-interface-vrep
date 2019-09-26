/**
(C) Copyright 2019 DQ Robotics Developers

This file is part of DQ Robotics.

    DQ Robotics is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DQ Robotics is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with DQ Robotics.  If not, see <http://www.gnu.org/licenses/>.

Contributors:
- Murilo M. Marinho        (murilo@nml.t.u-tokyo.ac.jp)
*/

#include<memory>

#include<dqrobotics/interfaces/YouBotVrepRobot.h>
#include<dqrobotics/utils/DQ_Constants.h>
#include<dqrobotics/robot_modeling/DQ_SerialManipulator.h>
#include<dqrobotics/robot_modeling/DQ_HolonomicBase.h>

namespace DQ_robotics
{

YouBotVrepRobot::YouBotVrepRobot(const std::string& robot_name, VrepInterface* vrep_interface): DQ_VrepRobot(robot_name, vrep_interface)
{
    std::vector<std::string> splited_name = strsplit(robot_name_,'#');
    std::string robot_label = splited_name[0];

    if(robot_label.compare(std::string("LBR4p")) != 0)
    {
        std::runtime_error("Expected LBR4p");
    }

    std::string robot_index("");
    if(splited_name.size() > 1)
        robot_index = splited_name[1];

    for(int i=1;i<8;i++)
    {
        std::string current_joint_name = robot_label + std::string("_joint") + std::to_string(i) + robot_index;
        joint_names_.push_back(current_joint_name);
    }
    base_frame_name_ = joint_names_[0];

}

DQ_robotics::DQ_WholeBody YouBotVrepRobot::kinematics()
{
    const double pi2 = pi/2.0;

    Matrix<double,4,5> dh(4,5);
    dh <<    0,      pi2,       0,      pi2,    0,
             0.147,    0,       0,        0,    0.218,
             0.033,    0.155,   0.135,    0,    0,
             pi2,      0,       0,      pi2,    0;


    auto arm = std::make_shared<DQ_SerialManipulator>(DQ_SerialManipulator(dh,"standard"));
    auto base = std::make_shared<DQ_HolonomicBase>(DQ_HolonomicBase());

    DQ x_bm = 1 + E_*0.5*(0.165*i_ + 0.11*k_);

    base->set_frame_displacement(x_bm);

    DQ_WholeBody kin(std::static_pointer_cast<DQ_Kinematics>(arm));
    kin.add(std::static_pointer_cast<DQ_Kinematics>(base));

    DQ effector = 1 + E_*0.5*0.3*k_;
    kin.set_effector(effector);

    return kin;
}

void YouBotVrepRobot::send_q_to_vrep(const VectorXd &q)
{
    vrep_interface_->set_joint_positions(joint_names_,q,VrepInterface::OP_ONESHOT);
}

VectorXd YouBotVrepRobot::get_q_from_vrep()
{
    return vrep_interface_->get_joint_positions(joint_names_,VrepInterface::OP_BUFFER);
}
}
