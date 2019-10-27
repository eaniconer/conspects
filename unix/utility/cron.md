
## Cron [_(wiki)_](https://en.wikipedia.org/wiki/Cron)

used to schedule jobs(command/scripts) to run periodically.

#### Usage example

Task: append `Hello` to file.txt once a minute

1. `crontab -e`  # opens editor to describe jobs
2. `* * * * * echo >> file.txt`  # job description

3. `crontab -r`  # removes all jobs

Note: `cronetab -l` show jobs

#### Syntax

`[minute 0-59] [hour 0-23] [day of month 1-31] [month 1-12] [day of week 0-6] command`

Note 1: `0` stands for Sunday.
Note 2: `*` stands for all possible numbers

##### Examples

`0 * * * * command` - once an hour at minute zero (00:00, 01:00, 02:00, 03:00)

`15 * * * * command` - 00:15, 01:15, 02:15

`30 2 * * * command` - every day at 02:30

`0 1 2 * * command` - Jan 2nd 01:00, Feb 2nd 01:00, etc

`0 * * * 1 command` - 00:00, 01:00, ..., but only on Mondays

`10,20 * * * * command` - two time every hour, at minutes 10 and 20

`*/5 * * * * command` - every 5 minutes (12 times per hour)

`0 5-10 * * * command` - run once every hour between 05:00 and 10:00

`@reboot command`- run command when cron rebooted


#### References
1. [Scheduling Tasks with Cron Jobs](https://code.tutsplus.com/tutorials/scheduling-tasks-with-cron-jobs--net-8800)
2. [The Beginners Guide To Cron Jobs](https://www.ostechnix.com/a-beginners-guide-to-cron-jobs/)

