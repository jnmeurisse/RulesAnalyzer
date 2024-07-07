# RulesAnalyzer
A firewall rules static analyzer.

# Introduction
RulesAnalyzer is a firewall rules static analyzer.  With this application, it is possible to identify misconfigured rules 
and rules that can be simplified.  It helps in finding and eliminating redundant, shadowed, or overly permissive firewall rules.

The core algorithm used in this application is described in the paper from Yuan, Lihua, et al. "Fireman: A toolkit for 
firewall modeling and analysis." 2006 IEEE Symposium on Security and Privacy (S&P'06). IEEE, 2006.

A firewall within the RulesAnalyzer application consists of a sequence of rules.  When evaluating rule anomalies, 
the application evaluates the rules in sequential order from the first to the last giving precedence to the first rule 
that matches a given traffic.  A firewall rule defines criteria such as source zones and addresses, destination zones and 
addresses, services, … that are used by a firewall to allow or deny a given traffic.  These criteria combined with an 
action (allow or deny) forms a rule.

The application currently supports the following set of criteria :
* Source zones and addresses : defines the origin of the traffic.
* Destination zones and addresses : defines the destination of the traffic.
* Services : defines the service protocol types (TCP, UDP or ICMP) as well as the logical destination ports (tcp/443 for example) of the traffic.
* Applications : define the applications generating the traffic.
* Users : define the users generating the traffic.

Important notes :
* RulesAnalyzer is a **static** analyzer.  RulesAnalyzer can only evaluate a criteria when all values are statically known.  RulesAnalyzer can only evaluate a criteria when all values are statically known.  This application supports addresses provided as a FQDN but can not take any decisions that are dynamically evaluated by a firewall such as URL wildcard filtering.
* When using an Application criteria, it is assumed that the modeled firewall uses protocol decoders that can analyze network traffic to detect application traffic even if the traffic uses non standard port or is encrypted.  The same applies for the User criteria, it is assumed that the modeled firewall has a mechanism to know which IP addresses map to which users.
* The RulesAnalyzer application supports only IPv4 addresses and TCP, UDP or ICMP protocol types.

RulesAnalyzer is an interactive application.  A basic command line interpreter allows you to interact with the application.
The command syntax is described here after.

---
# Anomalies
The command ```check anomaly``` can detect different types of configuration anomalies :

### Shadowing
Shadowing refers to the case where all the packets one firewall rule intends to deny (allow) have been accepted (denied) by one or a combination of preceding rules.  This is considered as a configuration error.

#### Example : rule 3 is shadowed by combined rules 1 and 2.

| id | action | source          | destination     | service        |
|:--:|--------|-----------------|-----------------|----------------|
| 1  | allow  | 192.168.10.0/24 | 192.168.20.0/24 | tcp/80         |
| 2  | allow  | 192.168.10.0/24 | 192.168.20.0/24 | tcp/443        |
| 3  | deny   | 192.168.10.10   | 192.168.20.10   | tcp/80;tcp/443 |

#### Output:
```
> fw load example_shadowing.csv
[info] loading rules from 'example_shadowing.csv'.
[info] 3 rows out of 3 loaded.
> fw check anomaly
[warning] a deny all rule is missing.
    +--+--------+--------+------------+-----+----------------------------------------+
    |id|src.zone|dst.zone|anomaly     |level|details                                 |
    +--+--------+--------+------------+-----+----------------------------------------+
    |3 |any     |any     |Fully masked|error|Shadowed rule                           |
    |  |        |        |            |     |packets are accepted by combined rules  |
    |  |        |        |            |     |1, 2                                    |
    +--+--------+--------+------------+-----+----------------------------------------+
```

### Redundancy
Redundancy refers to the case where if a firewall rule is removed, the firewall does not change its action on any packets. 
This is considered as a configuration error.

#### Example : rule 3 is redundant with combined rules 1 and 2.

| id | action | source          | destination     | service        |
|:--:|--------|-----------------|-----------------|----------------|
| 1  | allow  | 192.168.10.0/24 | 192.168.20.0/24 | tcp/80         |
| 2  | allow  | 192.168.10.0/24 | 192.168.20.0/24 | tcp/443        |
| 3  | allow  | 192.168.10.0/24 | 192.168.20.0/24 | tcp/80;tcp/443 |

#### Output:
```
> fw load example_redundancy.csv
[info] loading rules from 'example_redundancy.csv'.
[info] 3 rows out of 3 loaded.
> fw check anomaly
[warning] a deny all rule is missing.
    +--+--------+--------+------------+-----+----------------------------------------+
    |id|src.zone|dst.zone|anomaly     |level|details                                 |
    +--+--------+--------+------------+-----+----------------------------------------+
    |3 |any     |any     |Fully masked|error|Redundant rule                          |
    |  |        |        |            |     |packets are accepted by combined rules  |
    |  |        |        |            |     |1, 2                                    |
    +--+--------+--------+------------+-----+----------------------------------------+

```

### Generalization
Generalization refers to the case where a subset of the packets matched to this rule has been excluded by preceding rules. 
It is the opposite of shadowing and happens when a preceding rule matches a subset of this rule but takes a different action.
Generalization is considered as an anomaly warning.  Rules with generalizations can be ambiguous and difficult to maintain. 

#### Example : rule 3 is a generalization of combined rules 1 and 2.

| id | action | source          | destination    | service        |
|:--:|--------|-----------------|----------------|----------------|
| 1  | deny   | 192.168.10.10   | 192.168.20.0/24| tcp/80         |
| 2  | deny   | 192.168.10.20   | 192.168.20.0/24| tcp/443        |
| 3  | allow  | 192.168.10.20   | 192.168.20.0/24| tcp/80;tcp/443 |

#### Output:
```
> fw load example_generalization.csv
[info] loading rules from 'example_generalization.csv'.
[info] 3 rows out of 3 loaded.
> fw check anomaly
[warning] a deny all rule is missing.
    +--+--------+--------+----------------+-------+----------------------------------------+
    |id|src.zone|dst.zone|anomaly         |level  |details                                 |
    +--+--------+--------+----------------+-------+----------------------------------------+
    |3 |any     |any     |Partially masked|warning|Generalization of rules 1, 2            |
    +--+--------+--------+----------------+-------+----------------------------------------+
```

### Correlation
Correlation  refers to the case where the current rule intersects with preceding rules but specifies a different action.
Correlation is considered as an anomaly warning.  Rules with correlations can be ambiguous and difficult to maintain.

#### Example : rule 2 is correlated with rule 1
| id | action | source | destination    | service  |
|:--:|--------|--------|----------------|----------|
| 1  | allow  | any    | 192.168.1.0/24 | tcp/80   | 
| 2  | deny   | 10.1.1.0/24 | 192.168.0.0/16 | tcp/80   |

#### Output:
```
> fw load example_correlation.csv
[info] loading rules from 'example_correlation.csv'.
[info] 2 rows out of 2 loaded.
> fw check anomaly
[warning] a deny all rule is missing.
    +--+--------+--------+----------------+-------+----------------------------------------+
    |id|src.zone|dst.zone|anomaly         |level  |details                                 |
    +--+--------+--------+----------------+-------+----------------------------------------+
    |2 |any     |any     |Partially masked|warning|Correlated rule                         |
    |  |        |        |                |       |part of packets are accepted by rule 1  |
    +--+--------+--------+----------------+-------+----------------------------------------+
```
---
# Other deficiencies
In addition to the previous detections, the RuleAnalyzer application can detect other deficiencies :
### any/any
The command ```check any``` is capable of finding all firewall rules that allow network traffic to any destinations over any service 
protocol types and destination ports.

#### Example : rule1 allow any traffic

| id | action | source zone | source address |  destination zone | destination address  |service  |
|:--:|--------|-------------|----------------|----------|----------|----------|
| 1 | allow | lan | 10.0.3.31 | internet | any | any |
| 2| deny | any | any | any | any | any |

#### Output:
```
> fw load example_any.csv
[info] loading rules from 'example_any.csv'.
[info] 2 rows out of 2 loaded.
> fw check any
[info] 1 any/any rule found.
    +-------+---------+--------+-------------+--------+
    |rule id|rule name|src.zone|src.addr     |dst.zone|
    +-------+---------+--------+-------------+--------+
    |1      |         |lan     |10.0.3.31    |internet|
    +-------+---------+--------+-------------+--------+
```



### Symmetry
Symmetrical rules in a firewall refer to rules that allow the same traffic in both directions for a given connection.
Modern firewalls are stateful,  symmetrical rules are most of the time not necessary.  The command ```check symmetry``` is currently
restricted in finding single symmetrical rules.  It can not find a rule that is symmetrical to other combined rules.

#### Example : rule 1 and 3 are symmetrical

| id | action | source zone             | source address      |  destination zone | destination address  |service  |
|:--:|--------|-------------------------|---------------------|----------|----------|----------|
| 1 | allow | dmz                     | 192.168.100.23      | lan | 10.0.3.31 10.0.3.32| udp/123 |
| 2| allow| dmz                     | 192.168.100.23      | lan | 10.0.3.31 10.0.3.32 | tcp/445 |
| 3 | allow | lan | 10.0.3.31 10.0.3.32 | dmz | 192.168.100.23 | udp/123 |
| 4 | deny | any                     | any                 | any | any | any |

#### Output: shows symmetrical rules side by side
```
> fw load example_symmetry.csv
[info] loading rules from 'example_symmetry.csv'.
[info] 4 rows out of 4 loaded.
> fw check symmetry
    +------------+--------------+--------------+-------------------+--------------+
    |attribute   |name          |value         |name               |value         |
    +------------+--------------+--------------+-------------------+--------------+
    |name        |              |1             |                   |3             |
    +------------+--------------+--------------+-------------------+--------------+
    |status      |enabled       |1             |enabled            |1             |
    +------------+--------------+--------------+-------------------+--------------+
    |action      |allow         |1             |allow              |1             |
    +------------+--------------+--------------+-------------------+--------------+
    |src.zone    |dmz           |1             |corporate          |2             |
    +------------+--------------+--------------+-------------------+--------------+
    |dst.zone    |corporate     |2             |dmz                |1             |
    +------------+--------------+--------------+-------------------+--------------+
    |src.addr    |192.168.100.23|192.168.100.23|10.0.3.21-10.0.3.22|10.0.3.21/31  |
    +------------+--------------+--------------+-------------------+--------------+
    |dst.addr    |10.0.3.21     |10.0.3.21     |192.168.100.23     |192.168.100.23|
    |            |10.0.3.22     |10.0.3.22     |                   |              |
    +------------+--------------+--------------+-------------------+--------------+
    |services    |udp/53        |udp/53        |udp/53             |udp/53        |
    +------------+--------------+--------------+-------------------+--------------+
    |applications|any           |0-1023        |any                |0-1023        |
    +------------+--------------+--------------+-------------------+--------------+
    |users       |any           |0-1023        |any                |0-1023        |
    +------------+--------------+--------------+-------------------+--------------+
```

### deny all
The command ```check deny``` checks if a *deny all* rule is positioned at the bottom ensuring any traffic not explicitly allowed by previous rules is 
denied by default. 

---



