û# RulesAnalyzer
A firewall rules static analyzer.

# Introduction
RulesAnalyzer is a firewall rules static analyzer.  With this application, it is possible to identify misconfigured rules 
and rules that can be simplified.  It helps in finding and eliminating redundant, shadowed, or overly permissive firewall rules.

The core algorithm used in this application is described in the paper from Yuan, Lihua, et al. "Fireman: A toolkit for 
firewall modeling and analysis." 2006 IEEE Symposium on Security and Privacy (S&P'06). IEEE, 2006 (https://ieeexplore.ieee.org/document/1624012).

A firewall within the RulesAnalyzer application consists of a sequence of rules.  When evaluating rule anomalies, 
the application evaluates the rules in sequential order from the first to the last giving precedence to the first rule 
that matches a given traffic.  A firewall rule defines criteria such as source zones and addresses, destination zones and 
addresses, services, … that are used by a firewall to allow or deny a given traffic.  These criteria combined with an 
action (allow or deny) forms a rule.

The application currently supports the following set of criteria :
* Source zones and addresses : define the origin of the traffic.
* Destination zones and addresses : define the destination of the traffic.
* Services : define the service protocol types (TCP, UDP or ICMP) as well as the logical destination ports (tcp/443 for example) of the traffic.
* Applications : define the applications generating the traffic.
* Users : define the users generating the traffic.

> [!NOTE]
> * RulesAnalyzer is a **static** analyzer.  RulesAnalyzer can only evaluate a criteria when all values are statically known.  RulesAnalyzer can only evaluate a criteria when all values are statically known.  This application supports addresses provided as a FQDN but can not take any decisions that are dynamically evaluated by a firewall such as URL wildcard filtering.
> * When using an Application criteria, it is assumed that the modeled firewall uses protocol decoders that can analyze network traffic to detect application traffic even if the traffic uses non standard port or is encrypted.  The same applies for the User criteria, it is assumed that the modeled firewall has a mechanism to know which IP addresses map to which users.
> * The RulesAnalyzer application supports only IPv4 addresses and TCP, UDP or ICMP protocol types.

RulesAnalyzer is an interactive application.  A basic command line interpreter allows you to interact with the application.
The command syntax is described here after.

---
# Anomalies
The command `check anomaly` can detect different types of configuration anomalies :

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
| id | action | source      | destination    | service  |
|:--:|--------|-------------|----------------|----------|
| 1  | allow  | any         | 192.168.1.0/24 | tcp/80   | 
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
The command `check any` is capable of finding all firewall rules that allow network traffic to any destinations over
any service protocol types and destination ports.

#### Example : rule1 allow any traffic

| id | action  | source<br/>zone | source<br/>address | destination<br/>zone | destination<br/>address | service   |
|:--:|---------|-----------------|--------------------|----------------------|-------------------------|-----------|
| 1  | allow   | lan             | 10.0.3.31          | internet             | any                     | any       |
| 2  | deny    | any             | any                | any                  | any                     | any       |

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
Modern firewalls are stateful,  symmetrical rules are most of the time not necessary.  The command `check symmetry` 
is currently restricted in finding single symmetrical rules.  It can not find a rule that is symmetrical to other
combined rules.

#### Example : rule 1 and 3 are symmetrical

| id | action | source<br/>zone | source<br/>address  | destination<br/>zone | destination<br/>address | service |
|:--:|--------|-----------------|---------------------|----------------------|-------------------------|---------|
| 1  | allow  | dmz             | 192.168.100.23      | lan                  | 10.0.3.31 10.0.3.32     | udp/123 |
| 2  | allow  | dmz             | 192.168.100.23      | lan                  | 10.0.3.31 10.0.3.32     | tcp/445 |
| 3  | allow  | lan             | 10.0.3.31 10.0.3.32 | dmz                  | 192.168.100.23          | udp/123 |
| 4  | deny   | any             | any                 | any                  | any                     | any     |

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

### Deny all
The command `check deny` checks if a *deny all* rule is positioned at the bottom ensuring any traffic not explicitly
allowed by previous rules is denied by default. 

#### Example
| id | action | source<br/>zone | source<br/>address  | destination<br/>zone | destination<br/>address | service |
|:--:|--------|-----------------|---------------------|----------------------|-------------------------|---------|
| 1  | allow  | dmz             | 192.168.100.23      | lan                  | 10.0.3.31 10.0.3.32     | udp/123 |
| 2  | allow  | dmz             | 192.168.100.23      | lan                  | 10.0.3.31 10.0.3.32     | tcp/445 |
| 3  | deny   | any             | any                 | any                  | any                     | any     |

##### Output
```
> fw load example_deny_all.csv
[info] loading rules from 'example_deny_all.csv'.
[info] 3 rows out of 3 loaded.
> fw check deny
[info] deny all rule found, rule id = 3.
```

### Equivalence
Two sets of firewall rules are equivalent when the accepted traffic and the denied traffic are equivalent, meaning they
allow and block the same set of packets, applications and users.  The application is capable of modeling several
firewalls and therefore several sets of rules.  The `check equivalence` is used to compare a set of rules (or a subset
of rules) from one firewall with a set of rule (or subset of rules) from a second firewall. 

#### Example : two equivalent set of rules
- firewall #1

| id | action | source<br/>zone | source<br/>address | destination<br/>zone | destination<br/>address | service        |
|:--:|--------|-----------------|--------------------|----------------------|-------------------------|----------------|
| 1  | allow  | z1              | 192.168.10.0/24    | z2                   | 192.168.20.1            | tcp/80         |
| 2  | allow  | z1              | 192.168.10.0/24    | z2                   | 192.168.20.1            | tcp/443        |
| 3  | allow  | z1              | 192.168.10.0/24    | z2                   | 192.168.20.1            | tcp/80;tcp/443 |
| 4  | allow  | z1              | 192.168.11.0/30    | z2                   | 192.168.20.2            | udp/123        |
| 5  | allow  | z1              | 192.168.11.0/30    | z2                   | 192.168.20.2            | tcp/123        |
| 6  | deny   | z1              | 192.168.12.5       | z2                   | 192.168.20.2            | tcp/445        |
| 7  | allow  | z1              | any                | z2                   | 192.168.20.2            | tcp/445        |
| 8  | deny   | any             | any                | any                  | any                     | any            |

- firewall #2

| id | action | source<br/>zone | source<br/>address | destination<br/>zone | destination<br/>address | service         |
|:--:|--------|-----------------|--------------------|----------------------|-------------------------|-----------------|
| 1  | allow  | z1              | 192.168.10.0/24    | z2                   | 192.168.20.1            | tcp/80;tcp/443  |
| 2  | allow  | z1              | 192.168.11.0/30    | z2                   | 192.168.20.2            | udp/123;tcp/123 |
| 3  | allow  | z1              | !(192.168.12.5)    | z2                   | 192.168.20.2            | tcp/445         |
| 4  | deny   | any             | any                | any                  | any                     | any             |

Note : `!(192.168.12.5)` denotes the negation of address 192.168.12.5
#### Output:
```
> fw create fw1
[info] firewall 'fw1' created.
> fw load example_equivalence1.csv
[info] loading rules from 'example_equivalence1.csv'.
[info] 8 rows out of 8 loaded.
> fw create fw2
[info] firewall 'fw2' created.
> fw load example_equivalence2.csv
[info] loading rules from 'example_equivalence2.csv'.
[info] 4 rows out of 4 loaded.
> fw check equivalence fw1
Rules are equivalent.
```

---
# Modeling a firewall
Modeling a firewall consists of providing the list of rules and loading them into the application.
The following table shows the policy of a hypothetical firewall protecting a Web server in a DMZ and two database
servers in an internal zone.  Each rule is identified by a unique identifier.  The rule id is a unique positive number
that is used by the RuleAnalyzer application when reporting anomalies. 

| id | action | source<br/>zone | source<br/>address | destination<br/>zone | destination<br/>address | service        |
|:--:|--------|-----------------|--------------------|----------------------|-------------------------|----------------|
| 1  | allow  | Internet        | any                | DMZ                  | 192.168.100.23          | TCP/443        |
| 2  | allow  | DMZ             | 192.168.100.23     | Internet             | 8.8.8.8;8.8.4.4         | UDP/53;TCP/53  |
| 3  | allow  | DMZ             | 192.168.100.23     | LAN                  | 10.1.121.11;10.1.121.12 | TCP/5432       |
| 4  | allow  | LAN             | 10.0.0.0/8         | DMZ                  | 192.168.100.23          | TCP/443;TCP/22 |
| 5  | deny   | any             | any                | any                  | any                     | any            |

The RuleAnalyzer application currently supports the loading of firewall rules exclusively from CSV  (Comma-Separated
Values) files.  The structure of the CSV file and the required fields such as *id*, *action*, … are explained
later.

> [!NOTE]
> * *any* keyword is used in the context of universal quantification of a specific criteria.  It means that a specific
criteria will be verified for all values of a set.  For example, using any as the source address, means that the source
address criteria will be satisfied for all IPv4 addresses.
> * RuleAnalyzer assumes that a route exists between all zones.  In the previous example, we assume that the firewall
has an address translation mechanism configured that converts an IP address valid on the Internet zone to an address
valid on the DMZ zone.

The firewall can be modeled with the following .csv file :
```
id,action,src.zone,src.addr,dst.zone,dst.addr,svc
1,allow,internet,any,dmz,192.168.100.23,tcp/443
2,allow,dmz,192.168.100.23,internet,8.8.8.8;8.8.4.4,udp/53;tcp/53
3,allow,dmz,192.168.100.23,lan,10.1.121.11;10.1.121.12,tcp/5432
4,allow,lan,10.0.0.0/8,dmz,192.168.100.23,tcp/443;tcp22
5,deny,any,any,any,any,any
```
Modern firewalls provide objects that simplify the construction of rules.  A firewall object is a single object
(an address for example) or a group of objects.  The RuleAnalyzer application provides a dedicated object store
tailored for the storage of IPv4 addresses, address groups, services, service groups, applications, application
groups, users and user groups.  RuleAnalyzer offers commands designed for loading objects into the object store.  

> [!TIP]
> a zone is not considered as an object, it is not possible to create a group of zones.  If a policy allows  traffic from/to multiple zones, the firewall rules .csv file specifies the list of zones separated by a semicolon.

The same hypothetical firewall can be configured with objects:

| id | action | source<br/>zone | source<br/>address | destination<br/>zone | destination<br/>address | service    |
|:--:|--------|-----------------|--------------------|----------------------|-------------------------|------------|
| 1  | allow  | Internet        | any                | DMZ                  | WEB_SERVER              | https      |
| 2  | allow  | DMZ             | WEB_SERVER         | Internet             | GOOGLE_DNS              | dns        |
| 3  | allow  | DMZ             | WEB_SERVER         | LAN                  | DB_SERVERS              | PostgreSQL |
| 4  | allow  | LAN             | LAN_NET            | DMZ                  | WEB_SERVER              | https;ssh  |
| 5  | deny   | any             | any                | any                  | any                     | any        |

This firewall can be modeled with the 4 following .csv files :

1. Address .csv file ([example1_addr.csv](examples/example1_addr.csv))

    ```
    name,address 
    WEB_SERVER,192.168.100.23
    GOOGLE_DNS,8.8.8.8;8.8.4.4
    DB_SERVER_1,10.1.121.11 
    DB_SERVER_2,10.1.121.12
    LAN_NET,10.0.0.0/8
    ```

2. Address groups .csv file ([example1_addrg.csv](examples/example1_addrg.csv))
    ```
    name,members
    DB_SERVERS,DB_SERVER_1;DB_SERVER_2
    ```

3. Services .csv file ([example1_svc.csv](examples/example1_svc.csv))
    ```
    name,protoport
    https,tcp/443
    dns,udp/123;tcp/53
    ssh,tcp/22
    PostgreSQL,tcp/5432
    ```

4. Security firewall rules .csv file ([example1_rules.csv](examples/example1_rules.csv))
    ```
    id,action,src.zone,src.addr,dst.zone,dst.addr,svc
    1,allow,internet,any,dmz,WEB_SERVER,https
    2,allow,dmz,WEB_SERVER,internet,GOOGLE_DNS,dns
    3,allow,dmz,WEB_SERVER,lan,DB_SERVERS,PostgreSQL
    4,allow,lan,LAN_NET,dmz,WEB_SERVER,https;ssh
    5,deny,any,any,any,any,any
    ```

Example 1 : loading and showing the firewall rules

```
[info] object store cleared.
> os load addr example1_adr.csv
[info] loading objects from 'example1_adr.csv'.
[info] 5 objects loaded.
> os load addrg example1_adrg.csv
[info] loading objects from 'example1_adrg.csv'.
[info] 1 object loaded.
> os load svc example1_svc.csv
[info] loading objects from 'example1_svc.csv'.
[info] 4 objects loaded.
> fw load example1_rules.csv
[info] loading rules from 'example1_rules.csv'.
[info] 5 rows out of 5 loaded.
> fw show rules
    +--+------+--------+--------+--------------+--------------+--------+---+----+
    |id|action|src.zone|dst.zone|src.ip        |dst.ip        |svc     |app|user|
    +--+------+--------+--------+--------------+--------------+--------+---+----+
    |1 |allow |internet|dmz     |0.0.0.0/0     |192.168.100.23|tcp/443 |any|any |
    +--+------+--------+--------+--------------+--------------+--------+---+----+
    |2 |allow |dmz     |internet|192.168.100.23|8.8.8.8       |udp/123 |any|any |
    |  |      |        |        |              |8.8.4.4       |tcp/53  |   |    |
    +--+------+--------+--------+--------------+--------------+--------+---+----+
    |3 |allow |dmz     |lan     |192.168.100.23|10.1.121.11   |tcp/5432|any|any |
    |  |      |        |        |              |10.1.121.12   |        |   |    |
    +--+------+--------+--------+--------------+--------------+--------+---+----+
    |4 |allow |lan     |dmz     |10.0.0.0/8    |192.168.100.23|tcp/443 |any|any |
    |  |      |        |        |              |              |tcp/22  |   |    |
    +--+------+--------+--------+--------------+--------------+--------+---+----+
    |5 |deny  |any     |any     |0.0.0.0/0     |0.0.0.0/0     |any     |any|any |
    +--+------+--------+--------+--------------+--------------+--------+---+----+
```

When a firewall supports Application control, it is possible to describe which applications are allowed in a rule
in addition to the service ports.  Specifying “application-default” (or “app-default”) service in a firewall rule
means that the selected applications are allowed or denied only on their default services defined by the application
itself.  In the following example, the firewall rule n°3 is supposed to control the postgres protocol but restrict
it to a single destination port (5432).  The DNS protocol is allowed on any ports defined by the application
(udp/53, udp/5353 and tcp/53).


| id | action | source<br/>zone | source<br/>address | destination<br/>zone | destination<br/>address | application | service     |
|:--:|--------|-----------------|--------------------|----------------------|-------------------------|-------------|-------------|
| 1  | allow  | Internet        | any                | DMZ                  | WEB_SERVER              | any         | https       |
| 2  | allow  | DMZ             | WEB_SERVER         | Internet             | GOOGLE_DNS              | dns         | app-default |
| 3  | allow  | DMZ             | WEB_SERVER         | LAN                  | DB_SERVERS              | postgres    | PostgreSQL  |
| 4  | allow  | LAN             | LAN_NET            | DMZ                  | WEB_SERVER              | any         | https;ssh   |
| 5  | deny   | any             | any                | any                  | any                     | any         | any         |

The firewall can be modeled with the 5 following .csv files :
1. Addresses .csv file ([example2_addr.csv](examples/example2_addr.csv))
    ```
    name,address
    WEB_SERVER,192.168.100.23
    GOOGLE_DNS,8.8.8.8;8.8.4.4
    DB_SERVER_1,10.1.121.11
    DB_SERVER_2,10.1.121.12
    LAN_NET,10.0.0.0/8
    ```
2. Address groups .csv file ([example2_addrg.csv](examples/example2_addrg.csv))
    ```
    name,members
    DB_SERVERS,DB_SERVER_1;DB_SERVER_2
    ```
3. Applications .csv file ([example2_app.csv](examples/example2_app.csv))
    ```
    name,protoport
    postgres,tcp/5432;tcp/5434-5439
    dns,udp/53;udp/5353;tcp/53
    ```
4. Services .csv file ([example2_svc.csv](examples/example2_svc.csv))
    ```
    name,protoport
    https,tcp/443
    dns,udp/123;tcp/53
    ssh,tcp/22
    PostgreSQL,tcp/5432
    ```
5. Firewall rules .csv file ([example2_rules.csv](examples/example2_rules.csv))
    ```
    id,action,src.zone,src.addr,dst.zone,dst.addr,svc,app
    1,allow,internet,any,dmz,WEB_SERVER,https,any
    2,allow,dmz,WEB_SERVER,internet,GOOGLE_DNS,app-default,dns
    3,allow,dmz,WEB_SERVER,lan,DB_SERVERS,PostgreSQL,postgres
    4,allow,lan,LAN_NET,dmz,WEB_SERVER,https;ssh,any
    5,deny,any,any,any,any,any,any
    ```

Example 2 : loading and showing the firewall rules
```
> os load addr example2_addr.csv
[info] loading objects from 'example2_addr.csv'.
[info] 5 objects loaded.
> os load addrg example2_addrg.csv
[info] loading objects from 'example2_addrg.csv'.
[info] 1 object loaded.
> os load app example2_app.csv
[info] loading objects from 'example2_app.csv'.
[info] 2 objects loaded.
> os load svc example2_svc.csv
[info] loading objects from 'example2_svc.csv'.
[info] 4 objects loaded.
> fw load example2_rules.csv
[info] loading rules from 'example2_rules.csv'.
[info] 5 rows out of 5 loaded.
> fw show rules
    +--+------+--------+--------+--------------+--------------+--------+--------+----+
    |id|action|src.zone|dst.zone|src.ip        |dst.ip        |svc     |app     |user|
    +--+------+--------+--------+--------------+--------------+--------+--------+----+
    |1 |allow |internet|dmz     |0.0.0.0/0     |192.168.100.23|tcp/443 |any     |any |
    +--+------+--------+--------+--------------+--------------+--------+--------+----+
    |2 |allow |dmz     |internet|192.168.100.23|8.8.8.8       |udp/53  |dns     |any |
    |  |      |        |        |              |8.8.4.4       |udp/5353|        |    |
    |  |      |        |        |              |              |tcp/53  |        |    |
    +--+------+--------+--------+--------------+--------------+--------+--------+----+
    |3 |allow |dmz     |lan     |192.168.100.23|10.1.121.11   |tcp/5432|postgres|any |
    |  |      |        |        |              |10.1.121.12   |        |        |    |
    +--+------+--------+--------+--------------+--------------+--------+--------+----+
    |4 |allow |lan     |dmz     |10.0.0.0/8    |192.168.100.23|tcp/443 |any     |any |
    |  |      |        |        |              |              |tcp/22  |        |    |
    +--+------+--------+--------+--------------+--------------+--------+--------+----+
    |5 |deny  |any     |any     |0.0.0.0/0     |0.0.0.0/0     |any     |any     |any |
    +--+------+--------+--------+--------------+--------------+--------+--------+----+
```

When a firewall supports User control, it is also possible to describe which users are allowed in a firewall rule.
In the following example, accessing the web server from the LAN network is restricted to users belonging to the admins
group.   

| id | action | source<br/>zone | source<br/>address | destination<br/>zone | destination<br/>address | application | service     | user    | 
|:--:|--------|-----------------|--------------------|----------------------|-------------------------|-------------|-------------|---------|
| 1  | allow  | Internet        | any                | DMZ                  | WEB_SERVER              | any         | https       | any     |
| 2  | allow  | DMZ             | WEB_SERVER         | Internet             | GOOGLE_DNS              | dns         | app-default | any     |
| 3  | allow  | DMZ             | WEB_SERVER         | LAN                  | DB_SERVERS              | postgres    | PostgreSQL  | any     |
| 4  | allow  | LAN             | LAN_NET            | DMZ                  | WEB_SERVER              | any         | https;ssh   | admins  |
| 5  | deny   | any             | any                | any                  | any                     | any         | any         | any     |


The users are modeled with two additional .csv files : 
1. Users .csv file ([example3_usr.csv](examples/example3_usr.csv))
    ```
   name
    John Doe
    Alice Smith
    ```
2. User groups csv file ([example3_usrg.csv](examples/example3_usrg.csv))
    ```
    name,members
    admins,John Doe;Alice Smith
    ```
3. Firewall rules .csv file ([example3_rules.csv](examples/example3_rules.csv))
    ```
    id,action,src.zone,src.addr,dst.zone,dst.addr,svc,app,user
    1,allow,internet,any,dmz,WEB_SERVER,https,any,any
    2,allow,dmz,WEB_SERVER,internet,GOOGLE_DNS,app-default,dns,any
    3,allow,dmz,WEB_SERVER,lan,DB_SERVERS,PostgreSQL,postgres,any
    4,allow,lan,LAN_NET,dmz,WEB_SERVER,https;ssh,any,admins
    5,deny,any,any,any,any,any,any,any
    ```

Example 3 : loading and showing the firewall rules
```
> os load addr example3_addr.csv
[info] loading objects from 'example3_addr.csv'.
[info] 5 objects loaded.
> os load addrg example3_addrg.csv
[info] loading objects from 'example3_addrg.csv'.
[info] 1 object loaded.
> os load app example3_app.csv
[info] loading objects from 'example3_app.csv'.
[info] 2 objects loaded.
> os load svc example3_svc.csv
[info] loading objects from 'example3_svc.csv'.
[info] 4 objects loaded.
> os load usr example3_usr.csv
[info] loading objects from 'example3_usr.csv'.
[info] 2 objects loaded.
> os load usrg example3_usrg.csv
[info] loading objects from 'example3_usrg.csv'.
[info] 1 object loaded.
> fw load example3_rules.csv
[info] loading rules from 'example3_rules.csv'.
[info] 5 rows out of 5 loaded.
> fw show rules
    +--+------+--------+--------+--------------+--------------+--------+--------+-----------+
    |id|action|src.zone|dst.zone|src.ip        |dst.ip        |svc     |app     |user       |
    +--+------+--------+--------+--------------+--------------+--------+--------+-----------+
    |1 |allow |internet|dmz     |0.0.0.0/0     |192.168.100.23|tcp/443 |any     |any        |
    +--+------+--------+--------+--------------+--------------+--------+--------+-----------+
    |2 |allow |dmz     |internet|192.168.100.23|8.8.8.8       |udp/53  |dns     |any        |
    |  |      |        |        |              |8.8.4.4       |udp/5353|        |           |
    |  |      |        |        |              |              |tcp/53  |        |           |
    +--+------+--------+--------+--------------+--------------+--------+--------+-----------+
    |3 |allow |dmz     |lan     |192.168.100.23|10.1.121.11   |tcp/5432|postgres|any        |
    |  |      |        |        |              |10.1.121.12   |        |        |           |
    +--+------+--------+--------+--------------+--------------+--------+--------+-----------+
    |4 |allow |lan     |dmz     |10.0.0.0/8    |192.168.100.23|tcp/443 |any     |John Doe   |
    |  |      |        |        |              |              |tcp/22  |        |Alice Smith|
    +--+------+--------+--------+--------------+--------------+--------+--------+-----------+
    |5 |deny  |any     |any     |0.0.0.0/0     |0.0.0.0/0     |any     |any     |any        |
    +--+------+--------+--------+--------------+--------------+--------+--------+-----------+
```

# The command line interface
RuleAnalyzer is an interactive application.  It enables you to type in commands and see the results.  Most of the
commands have also a short form.  All results are presented in an ASCII-table format.  It is possible to save the
result into a text file or a .CSV file when the command supports the -o option.

It is also possible to put RuleAnalyzer commands in a file and read its input from that file.
## Object store commands
### Importing objects from a CSV file.
Importing firewall objects is only possible from a CSV file.  The RuleAnalyzer application always reports an error if
an object does not have the correct format.  If an object is already defined in the object store, its definition is
overwritten when loading a new object.

Object names stored in the RuleAnalyzer application are case-sensitive. This means that "Object1" and "object1" are 
treated as two different object names. It's important to use the correct case for the object names to ensure a correct
model.

The first row of a .csv file must contain parameter names (column names). Each row below the first row contains
parameter values.  Column names and parameter values must be separated by a coma (,).  
#### Addresses
* `ostore load address csvfile` *(short form: os l addr csvfile)*   
Loads addresses from the .csv file into the address objects store.  An address is loaded from parameters *name*, *type* 
and *address*.  The *name* is the object name.  The *type* represents the type of address stored in the address column.
The accepted types are ipmask, iprange or fqdn :
    * **ipmask** type — the address parameter contains a list of IPv4 addresses separated by a semicolon.  Each address
    must conform to the following syntax :  
        * a single IP address : 192.168.2.43 or 192.168.2.43/32
        * a subnet : 192.168.2.0/24 
        * any address : any
    * **iprange** type — the address parameter contains a list of IPv4 ranges separated by a semicolon.  Each address
    must conform to the following syntax :  
    a range of addresses : 192.168.2.1-192.168.2.10
    * **fqdn** type — the address parameter contains a single hostname.
  
    The *type* parameter is optional.  The application tries to detect the type when this parameter is missing in
the .csv file.  A fqdn address is resolved to an IP address when loading a firewall rule that refers to this address. The application
maintains a cache of resolved fqdn addresses in a local file (by default, rulesanalyzer.fqdn).  Fqdn addresses that
are not resolved are not stored in the cache.  It is possible to completely disable the cache but also customize
the filename and file location.  

* `ostore load address-group csvfile` *(short form: os l addrg csvfile)*  
Loads address groups from the .csv file into the address group objects store.  An address group is loaded from the
parameters *name* and *members*.  The coherence of an address group object is validated only when a firewall rule
referring to this group is imported into the application.

#### Services
* `ostore load service csvfile` *(short form: os l svc csvfile)*  
Loads services from the .csv file into the service objects store.  A service is loaded from parameters *name* and
*protoport*.  The *name* is the object name and *protoport* is a list of protocol/port combinations separated
by a semicolon.  A *protoport* in the list must conform to the following syntax :  
    * a specific protocol/port combination  : protocol{/range} with      
	protocol ::= tcp | udp | icmp    
	range ::= port | port-port    
    and port is an integer value.
    * any service : any

    Examples : 
    ```
    name,protoport
    UDP,udp
    SSH,tcp/22
    CUSTOM,tcp/32000-32002
    PING,icmp/8
    ```

* `ostore load service-group csvfile` *(short form: os l svcg csvfile)*    
Loads service groups from the .csv file into the service group objects store.  A service group is loaded from the
parameters *name* and *members*.  The coherence of a service group object is validated only when a firewall rule
referring to this group is imported into the application.

#### Applications
* `ostore load application csvfile` *(short form: os l app csvfile)*  
Loads applications from the .csv file into the application objects store.  An application is loaded from the parameters
*name* and *protoport*.  The *name* is the object name and *protoport* is a list of standard protocol/port combinations
separated by a semicolon.

* `ostore load application-group csvfile` *(short form: os l appg csvfile)*  
Loads application groups from the .csv file into the application group objects store.  An application group is loaded
from the parameters *name* and *members*.  The coherence of an application group object is validated only when a firewall
rule referring to this group is imported into the application.

#### Users
* `ostore load user csvfile` *(short form: os l usr csvfile)*  
Loads users from the .csv file into the user objects store.  A user is loaded from the parameter *name*.  The name is 
the object name.

* `ostore load user-group csvfile` *(short form: os l usrg csvfile)*
Loads user groups from the .csv file into the user group objects store.  An user group is loaded from the parameters
*name* and *members*.  The coherence of an application group object is validated only when a firewall rule referring
to this group is imported into the application.

### Querying the object store
The querying command may be applied to the miscellaneous object types.  The query command is case-insensitive and
supports wildcards.  
  
* `ostore query address query-string` *(short form: os q addr query-string)*
* `ostore query address-group query-string` *(short form: os q addrg query-string)*
* `ostore query service query-string` *(short form: os q svc query-string)*
* `ostore query service-group query-string` *(short form: os q svcg query-string)*
* `ostore query application query-string` *(short form: os q app query-string)*
* `ostore query application-group query-string` *(short form: os q appg query-string)*
* `ostore query user query-string` *(short form: os q usr query-string)*
* `ostore query user-group query-string` *(short form: os q usrg query-string)*


Example: 
```
> os l addr example3_adr.csv
[info] loading objects from 'example3_adr.csv'.
[info] 5 objects loaded.
> os q addr google*
    +----------+----------------------------------------+
    |name      |value                                   |
    +----------+----------------------------------------+
    |GOOGLE_DNS|8.8.8.8, 8.8.4.4                        |
    +----------+----------------------------------------+
> os q addr *1
    +-----------+----------------------------------------+
    |name       |value                                   |
    +-----------+----------------------------------------+
    |DB_SERVER_1|10.1.121.11                             |
    +-----------+----------------------------------------+
```
### Other object store commands
* `ostore clear` *(short form: os clear)*  
Clears the object store.  All objects are deleted from the object store.  

* `ostore info` *(short form: os info)*
Shows how many objects are stored in the object store.

## Firewall commands
It is possible to load rules from a CSV file that was created manually or exported from a firewall supporting that functionality.  

* `firewall load csvfile` *(short form: fw l csvfile)*  
Loads firewall rules from the .csv file into the current firewall.  A firewall rule is loaded from the parameters :

| Parameter  | Description                                                                                                                                                                                                                             | Multiple | Optional | Default | 
|------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------|----------|---------| 
| id         | The rule id is a unique identifier assigned to each firewall rule.<br/>It ensures that every rule can be individually referenced.                                                                                                       | No       | No       |         |
| name       | The rule name is a descriptive label for the rule.                                                                                                                                                                                      | No       | Yes      |         |
| status     |                                                                                                                                                                                                                                         | No       | Yes      | enable  |
| action     | The action specifies what the firewall should do when a packet matches the criteria defined in the rule.<br/>The rule loader accepts the following values for the accept parameter: allow,  accept, deny, drop.                         | No       | No       |         |
| src.zone   | Defines the origin points for traffic in the firewall rule.<br/>A source zone refers to logical groupings of network interfaces or segments                                                                                             | Yes      | Yes      | any     |
| src.addr   | Defines the source addresses from which incoming traffic is accepted (or denied).<br/>A source address can be an address object or an address group object.  Multiple addresses can be specified separated by a semicolon.              | Yes      | No       |         |
| src.negate | A boolean that is used to invert the source addresses criteria.<br/>When true, the source address criteria is verified when the source traffic does not come from src.addr.                                                             | Yes      | Yes      | false   |
| dst.addr   | Defines the destination addresses to which outgoing traffic is accepted (or denied).<br/>A destination address can be an address object or an address group object.  Multiple address groups can be specified separated by a semicolon. | Yes      | No       |         |
| dst.negate | A boolean that is used to invert the destination addresses criteria.<br/>When true, the destination address criteria is verified when the destination traffic does not come from src.addr.                                              | Yes      | Yes      | false   |
| svc        | Defines the services                                                                                                                                                                                                                    | Yes      | No       |         |
| app        |                                                                                                                                                                                                                                         | Yes      | Yes      | any     |
| user       |                                                                                                                                                                                                                                         | Yes      | Yes      | any     |


If there is a failure while importing a rule, the reason will be given and the rule will be skipped.  For example, rules
that make reference to objects (address, service, …) that are not available in the object store are skipped.

* `firewall save csvfile | txtfile`
* `firewall create name`
* `firewall delete name`
* `firewall select name`
* `firewall enable rule id`
* `firewall disable rule id`
* `firewall info`

### Show commands
All show commands are applied to the current firewall and enabled rules.

* `firewall show zones` *(short form: fw sh zones)*  
Displays all zones that are configured on the current firewall.  The output table shows how many allow and deny rules
are configured between two zones.

Example :







* `firewall show rule id [id]`
(short form: fw sh rule  id [id])

* `firewall show rules [-z source-zone destination-zone] [-o filename]`
(short form: fw sh rules [-z source-zone destination-zone] [-o filename])

* `Firewall show address [-z source-zone destination-zone] [-o filename]`
(short form: fw sh addr [-z source-zone destination-zone] [-o filename])

* `firewall show service [-z source-zone destination-zone] [-o filename]`
(short form: fw sh svc [-z source-zone destination-zone] [-o filename])

* `firewall show application [-z source-zone destination-zone] [-o filename]`
(short form: fw sh app [-z source-zone destination-zone] [-o filename])

* `firewall show user [-z source-zone destination-zone] [-o filename]`
(short form: fw sh usr [-z source-zone destination-zone] [-o filename])

### Check commands
All check commands are applied to the current firewall and enabled rules.

* `firewall check any [address] [-z source-zone destination-zone]`

* `firewall check deny [-z source-zone destination-zone]`

* `firewall check anomaly [-z source-zone destination-zone]`

* `firewall check symmetry [-z source-zone destination-zone] [-o filename]`

* `firewall check equivalence firewall [-z source-zone destination-zone]`

* `Firewall check address address [-z source-zone destination-zone] [-o filename]`
(short form: fw ch addr address [-z source-zone destination-zone] [-o filename])

* `firewall check service service [-z source-zone destination-zone]`
(short form: fw ch svc service [-z source-zone destination-zone])

* `firewall check application application [service] [-z source-zone destination-zone]`
(short form: fw ch app application [service] [-z source-zone destination-zone])

* `firewall check user user [-z source-zone destination-zone]`
(short form: fw ch usr user [-z source-zone destination-zone])
