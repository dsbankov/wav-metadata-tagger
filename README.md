# wav-metadata-tagger
Command line tool for adding BWF metadata to WAV files.

Example usage: .\wav-metadata-tagger.exe --path "..\Adrift Break.wav" --metadata PROJECT=DrumBreaks02 --metadata NOTE="Some notes" --metadata BWF_DESCRIPTION="all the old stuff" --metadata MusicalTempo=174 --metadata MediaAlbum="album name"

## Build
Open the project with Visual Studio 2019 and build.

## Supported Metadata
#### Basic iXML Metadata
- NOTE
- PROJECT
- SCENE
- TAKE
- TAPE
#### BEXT Chunk Metadata
- BWF_CODING_HISTORY
- BWF_DESCRIPTION
- BWF_ORIGINATION_DATE
- BWF_ORIGINATION_TIME
- BWF_ORIGINATOR_REFERENCE
- BWF_ORIGINATOR
#### BaseHead / Steinberg's Mediabay Metadata
- MusicalInstrument
- MediaRecordingMethod
- MediaArtist
- MediaCategoryPost
- MediaLibraryName
- MusicalTempo
- MediaAlbum
- AudioCDName
- MediaComment
- AudioActor
- SmfCopyRight
- MediaTrackNumber
- AudioSoundEditor
- AudioEpisode
- MediaGenre
- MediaRecordingLocation
- MediaLibraryManufacturerName
- MusicalCharacter
- MediaCompany
- AudioSoundMixer
- MediaTemplateCategory
- MusicalStyle
- MusicalCategory
- MediaTrackTitle
