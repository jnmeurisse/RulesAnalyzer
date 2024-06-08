# RulesAnalyzer
A firewall rules static analyzer.

# Introduction
RulesAnalyzer is a firewall rules static analyzer.  With this application, it is possible to identify misconfigured rules and rules that can be simplified.  It helps in finding and eliminating redundant, shadowed, or overly permissive firewall rules.

The core algorithm used in this application is described in the paper from Yuan, Lihua, et al. "Fireman: A toolkit for firewall modeling and analysis." 2006 IEEE Symposium on Security and Privacy (S&P'06). IEEE, 2006.

A firewall within the RulesAnalyzer application consists of a sequence of rules.  When evaluating rule anomalies, the application evaluates the rules in sequential order from the first to the last giving precedence to the first rule that matches a given traffic.  A firewall rule defines criteria such as source zones and addresses, destination zones and addresses, services, … that are used by a firewall to allow or deny a given traffic.  These criteria combined with an action (allow or deny) forms a rule.

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

RulesAnalyzer is an interactive application.  A basic command line interpreter allows you to interact with the application.  The command syntax is described here after.

# Anomalies
The application can detect different types of configuration anomalies :

## Shadowing
Shadowing refers to the case where all the packets one firewall rule intends to deny (allow) have been accepted (denied) by one or a combination of preceding rules.  This is considered as a configuration error.

Example :

| id | action |source |destination | service |
| -- | -- | -- | -- | -- |
| 1  | allow | 192.168.10.0/24 |192.168.20.0/24 | tcp/80 |
| 2  | allow | 192.168.10.0/24 |  192.168.20.0/24 | tcp/443 |
| 3  | deny  | 192.168.10.10 | 192.168.20.10 | tcp/80,tcp/443 |

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
