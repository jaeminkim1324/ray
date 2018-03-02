#ifndef SCHEDULING_POLICY_CC
#define SCHEDULING_POLICY_CC

#include "scheduling_policy.h"

#include <unordered_map>

#include "ray/raylet/scheduling_resources.h"

using namespace std;
namespace ray{

SchedulingPolicy::SchedulingPolicy(const SchedulingQueue &sched_queue)
  : sched_queue_(sched_queue) {}

std::unordered_map<TaskID, ClientID, UniqueIDHasher> SchedulingPolicy::Schedule(
    const std::unordered_map<ClientID, SchedulingResources, UniqueIDHasher>
    &cluster_resources) {

  static ClientID local_node_id = ClientID::nil();
  std::unordered_map<TaskID, ClientID, UniqueIDHasher> decision;
  // TODO(atumanov): consider all cluster resources.
  SchedulingResources resource_supply = cluster_resources.at(local_node_id);
  const auto &resource_supply_set = resource_supply.GetAvailableResources();

  // Iterate over running tasks, get their resource demand and try to schedule.
  for (const auto &t : sched_queue_.ready_tasks()) {
    // Get task's resource demand
    const auto &resource_demand = t.GetTaskSpecification().GetRequiredResources();
    bool task_feasible = resource_demand.isSubset(resource_supply_set);
    if (task_feasible) {
      const TaskID &task_id = t.GetTaskSpecification().TaskId();
      decision[task_id] = local_node_id;
    }
  }
  return decision;
}

SchedulingPolicy::~SchedulingPolicy() {}

} // namespace ray

#endif // SCHEDULING_POLICY_CC
