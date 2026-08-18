# projectnelth
 Project Neltharion 4.3.4, Full Oct 2023 Release version


Program versions used by our project:

- MariaDB 10.4: https://archive.mariadb.org//mariadb-10.4.7/winx64-packages/mariadb-10.4.7-winx64.msi
  (Does not work in x86 folder)
- Openssl 1.0.2u: (64) https://github.com/openssl/openssl/releases/download/OpenSSL_1_0_2u/openssl-1.0.2u.tar.gz
- Cmake 3.25.2(required to specify visual studio 2022): https://github.com/Kitware/CMake/releases/download/v3.25.2/cmake-3.25.2-windows-x86_64.msi
- Visual Studio 2022 (17): https://aka.ms/vs/17/release/vs_community.exe
- (Optional, Recommended) [HeidiSQL database client](https://release-assets.githubusercontent.com/github-production-release-asset/109230350/01eb734f-ec46-4145-be10-8be5bc93abf8?sp=r&sv=2018-11-09&sr=b&spr=https&se=2026-08-13T00%3A03%3A38Z&rscd=attachment%3B+filename%3DHeidiSQL_12.21.0.7344_Setup.exe&rsct=application%2Foctet-stream&skoid=96c2d410-5711-43a1-aedd-ab1947aa7ab0&sktid=398a6654-997b-47e9-b12b-9515b896b4de&skt=2026-08-12T23%3A03%3A02Z&ske=2026-08-13T00%3A03%3A38Z&sks=b&skv=2018-11-09&sig=5W7CuxhtT0u0tZ4fDZQ2oqNsQYd4KNXv%2BgHGd4ofG9s%3D&jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmVsZWFzZS1hc3NldHMuZ2l0aHVidXNlcmNvbnRlbnQuY29tIiwia2V5Ijoia2V5MSIsImV4cCI6MTc4NjU3ODI2MywibmJmIjoxNzg2NTc2NDYzLCJwYXRoIjoicmVsZWFzZWFzc2V0cHJvZHVjdGlvbi5ibG9iLmNvcmUud2luZG93cy5uZXQifQ.14K53ePFdOQ0WW7Fj0Pt4dZTZQWuS7A_wAZ0uPWJGiE&response-content-disposition=attachment%3B%20filename%3DHeidiSQL_12.21.0.7344_Setup.exe&response-content-type=application%2Foctet-stream)


# How-To
1. Clone repository to machine
2. Download listed programs, note the database login given for mariaDB.
3. Open project/bin/relwithdebinfo/data, install and unpack downloads in that location.
   <img width="651" height="288" alt="image" src="https://i.imgur.com/yvyoDDD.png" />
4. Open source/sql/base, install and unpack downloads in that location.
   <img width="1377" height="697" alt="image" src="https://i.imgur.com/ess3Sls.png" />
5. Open mariadb/heidisql, create databases "Auth", "Characters", "World" as UTF-8 GeneralCi.
   <img width="1807" height="1508" alt="image" src="https://i.imgur.com/yP4J7UD.png" />
6. Select each database, run their respective SQL file to flesh out databases. Requires disconnecting to see table contents.
   <img width="2394" height="1480" alt="image" src="https://i.imgur.com/pcEcegt.png" />
7. Open cmake, fill out information shown, rerun "Configure" with valid info until all options are showing. Generate until success.
   <img width="1807" height="1508" alt="image" src="https://i.imgur.com/jAwtsRy.png" />
8. Open Trinitycore.sln with visual studio 2022. Clean solution, build solution, clean solution, rebuild solution. With a fully compiled core, re-attempts should complete very fast. When successful your relwithdebinfo folder should look like as shown below.
<img width="985" height="793" alt="image" src="https://i.imgur.com/umdgvs3.png" />
9. Rename authserver.conf.dist to authserver.conf.
10. Rename worldserver.conf.dist to worldserver.conf.
11. Open worldserver and authserver.conf files, correct the database login information.


#What is it?

Project Neltharion, as most recently existed in October 2023.
This project is the result of four years of veteran player collaboration, with a focus on endgame replay value and solving long-standing problems.
We implented features, systems, checks, and rules that never existed on blizzard but made a point to keep the game recognizable on the surface to avoid negative effects on players looking for a strictly blizzlike server.

This release is "final", as there will be no further support by myself or under the Project Neltharion name.
The final product is not perfect, as finding more imperfections than you can fix is a natural part of reverse engineering a game. 
The 4 year changelog may be impressive, but our project also started with a source we felt, at the time, was 'almost' complete before spending 4 years working on it.

"What is the core based off?"
- 2019 beta launch was a Chimera of OMFG private-leaked core and Paragon-WoW (FR) scripts. I cannot link as I am not the one who sourced them.
- Anticheat pulled from (Azerothcore?) WOTLK and fitted to cataclysm by Dandi
- M+ System pulled from (Unkown) Legion and fitted to cataclysm by Traesh
- All focus-content scripts and systems received hundreds if not thousands of hours of polishing work.

Focused Content:
- Endgame 85 Gameplay Loop (CLass Mechanics, Arena, BGs, WPvP, Dungeons, Raids, M+)
- Critical Systems & QOL
- Starting & chokepoint quest zones

Neglected Content (Original to source cores):
- Pre-Cata Dungeons & Raids
- Quest zones where leveling in them is optional
- Profession Gathering

For the short list of issues I had in mind from the project's final days, Check the known issues file.
For the full public changelog leading up to the october 6 2023 release, check the file next to the main readme.
For website support with this core, contact the core's primary web developer Darksider (Discord: d4rksid3r, 172643476029177856)
