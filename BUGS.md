Баги
====

* крайне коряво автоматически создает главы (последствия импорта)
** В чем была причина: если в project.ini нету настроек settings'са, то они и не загружаются не от куда. Поэтому не стоят нужные галки: Do not ask for item properties when creating ( CBItemAutoProperties )
