/// -*- coding: utf-8 -*-
///
/// file: zmModel.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef FLOWMODEL_H
#define FLOWMODEL_H

#include "zmChangeSet.h"

#include <map>
#include <string>

namespace zm
{
    class MindMatter;

    class MindMatterModel
    {
    public:

        typedef std::map< std::string, MindMatter * > MindMatterModelMapType;

    private:

        MindMatterModelMapType  m_things;
        std::string         m_localFolder;
        std::string         m_localModelFile;
        ChangeSet           m_changeSet;

        std::string         m_temporaryJournalFile;
        bool                m_initialized;
        bool                m_dirty;  // maybe we need more

    public:

        MindMatterModel();

        void setConfigPersistance( bool value );

        /// sets the one and only local folder for configuration temorary and
        /// snapshot files
        void setLocalFolder( const std::string &path );

        /// defines a (externally synced) folder exchange with other clients
        /// and/or users
        void addDomainSyncFolder( const std::string &domainName, const std::string &path );

        /// will initialize the model by loading the persistant state using
        /// the given information
        void initialize();

        /// will write recent changes to the journal file
        void localSave();

        /// make a fulf model sync.
        /// first load and apply new journal files in the sync folders. Then
        /// write the local model files and make the temporary journal file
        /// available to the sync folders and
        void sync();

        /// development only: load an external model file and merge items
        /// which don't exist in the current model
        void merge( const std::string &modelFile );

        /// returns whether there is a valid username stored
        bool hasUsedUsername() const;

        /// returns whether there is a valid hostname stored
        bool hasUsedHostname() const;

        /// returns the username used by us
        std::string getUsedUsername() const;

        /// set the username used by us - should be the system user
        /// name but doesn't have to
        void setUsedUsername( const std::string &username );

        /// returns the hostname used by us - should be the system user
        /// name but doesn't have to
        std::string getUsedHostname() const;

        /// set the hostname used by us
        void setUsedHostname( const std::string &hostname );

    private:

        void loadLocalModel( const std::string &filename );

        bool importJournalFiles();

        void dirty();

        void saveLocalModel( const std::string &filename );

        void makeTempJournalStatic();

        /// returns a sorted list containing names of journal files
        /// located in the sync folder
        std::vector< std::string > getJournalFiles();

        void applyChangeSet( const ChangeSet &changeSet );

    /// const interface
    public:

        const MindMatterModelMapType & things() const;

        bool equals( const MindMatterModelMapType &thingsMap, const MindMatterModelMapType &thingsMapOther ) const;

        size_t getItemCount() const;

        std::time_t getCreationTime( const std::string &uid ) const;

        bool hasItem( const std::string &uid ) const;

        std::string getValue( const std::string &uid, const std::string &name ) const;

        const std::string & getCaption( const std::string &uid ) const;

        bool hasAttribute( const std::string &uid, const std::string &attribute ) const;

        bool hasValue( const std::string &uid, const std::string &name ) const;

        bool itemContentMatchesString( const std::string &uid, const std::string &searchString ) const;

        bool isConnected( const std::string &node1_uid, const std::string &node2_uid ) const;

    /// write relevant interface
    public:

        std::string createNewItem( const std::string &caption );

        void eraseItem( const std::string &uid );

        void addAttribute( const std::string &uid, const std::string &attribute );

        bool removeAttribute( const std::string &uid, const std::string &attribute );

        void setValue( const std::string &uid, const std::string &name, const std::string &value );

        void setCaption( const std::string &uid, const std::string &caption );

        void connect( const std::string &node1_uid, const std::string &node2_uid );

    private:

        void _createNewItem( const std::string &uid, const std::string &caption, const std::string &time );

        void _eraseItem( MindMatterModelMapType::iterator &item );

        void _addAttribute( MindMatterModelMapType::iterator &item, const std::string &attribute );

        bool _removeAttribute( MindMatterModelMapType::iterator &item, const std::string &attribute );

        bool _setValue( MindMatterModelMapType::iterator &item, const std::string &name, const std::string &value );

        bool _setCaption( MindMatterModelMapType::iterator &item, const std::string &caption );

        void _connect( MindMatterModelMapType::iterator &item1, MindMatterModelMapType::iterator &item2 );

        // returns 16x8 bit
        static std::string generateUid();

        static void clear( MindMatterModelMapType &thingsMap );
    };
}

#endif
