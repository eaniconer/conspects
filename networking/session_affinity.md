# Session Affinity

Допустим, что у нас есть несколько серверов и балансировщик нагрузки. При отправке клиентом запроса, балансировщик по некоторому алгоритму выбирает сервер и перенаправляет запрос ему.

`Session Affinity` позволяет сделать так, чтобы все последующие запросы от клиента, направлялись тому же серверу. 

При первом запросе от клиента, сервер сохраняет идентификатор сессии и информацию о сервере в `cookie`. 
 - Идентификатор сессии позволяет серверу найти информацию о текущей сессии
 - Информация о сервере позволяет балансировщику выбрать нужный сервер

Плюсы:
 - Не нужно передавать данные между разными серверами
 - Эффективное использование кеша оперативной памяти
 - Уменьшение количества обращение к базе данных за счет кеширования и хранения состояния локально
 - Приложение с таким подходом (session per server) проще тестировать и запускать локально
  
Про некоторые минусы подхода описано [здесь](https://dev.to/gkoniaris/why-you-should-never-use-sticky-sessions-2pkj): 
 - Если сервер с данными сессии упал или завис, то могут возникнуть проблемы с тем, чтобы получить эти данные другим сервером.
 - Уязвимость: можно поочередно проводить DOS атаки на сервера: нагрузка не будет распределяться между серверами, а будет идти на один конкретный сервер.

### `Session Affinity` vs `Sticky Session`
*Кажется, что это одно и тоже, но бывают разные представления об этом:*

1. https://stackoverflow.com/questions/1040025/difference-between-session-affinity-and-sticky-session
2. https://community.ibm.com/community/user/imwuc/communities/community-home/digestviewer/viewthread?GroupId=19&MID=3381&CommunityKey=5c4ba155-561a-4794-9883-bb0c6164e14e&tab=digestviewer&ReturnUrl=%2Fcommunity%2Fuser%2Fimwuc%2Fcommunities%2Fcommunity-home%2Fdigestviewer%3FCommunityKey%3D5c4ba155-561a-4794-9883-bb0c6164e14e

### References

1. Как поддерижвается session affinity на Cloudflare [[link]](https://developers.cloudflare.com/load-balancing/understand-basics/session-affinity/)
2. ibm explanation [[link]](https://www.ibm.com/support/knowledgecenter/SS9H2Y_7.6.0/com.ibm.dp.doc/lbg_sessionaffinity.html)
3. heroku explanation [[link]](https://devcenter.heroku.com/articles/session-affinity)
4. imperva explanation [[link]](https://www.imperva.com/learn/availability/sticky-session-persistence-and-cookies/)

