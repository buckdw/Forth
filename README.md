5 100 !    
72 101 !    
69 102 !    
76 103 !    
76 104 !    
79 105 !    
100 COUNT

I've written an ansible playbook to orchestrate azdo pipelines. 
The orchestrator performs status check of pipelines. 
I'm not trying to copy the azdo statemodel of pipelines. 
Within our orchestrator a pipeline is either INITIAL, RUNNING, DONE or FAILED. 

I want to check for end-states DONE and FAILED. 
My idea was to create 2 facts called completed and failed. 
Completed is true when status_resu;lt.status == completed and status_result.result == succeeded. 
I want to use these 2 facts to conditionally execute an sql statement that updates the status. 
Can you generate the logic for this
