//
// Generated file, do not edit! Created by nedtool 5.0 from gPtpPacket.msg.
//

#ifndef __GPTPPACKET_M_H
#define __GPTPPACKET_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0500
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
//#include "simtime.h"
#include "gPtp.h"
#include "omnetpp.h"
// }}

/**
 * Class generated from <tt>gPtpPacket.msg:17</tt> by nedtool.
 * <pre>
 * packet gPtp_Sync
 * {
 *     omnetpp::SimTime sentTime;
 * }
 * </pre>
 */
class gPtp_Sync : public ::omnetpp::cPacket
{
  protected:
    omnetpp::SimTime sentTime;

  private:
    void copy(const gPtp_Sync& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const gPtp_Sync&);

  public:
    gPtp_Sync(const char *name=nullptr, int kind=0);
    gPtp_Sync(const gPtp_Sync& other);
    virtual ~gPtp_Sync();
    gPtp_Sync& operator=(const gPtp_Sync& other);
    virtual gPtp_Sync *dup() const {return new gPtp_Sync(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b);

    // field getter/setter methods
    virtual omnetpp::SimTime& getSentTime();
    virtual const omnetpp::SimTime& getSentTime() const {return const_cast<gPtp_Sync*>(this)->getSentTime();}
    virtual void setSentTime(const omnetpp::SimTime& sentTime);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const gPtp_Sync& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, gPtp_Sync& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>gPtpPacket.msg:22</tt> by nedtool.
 * <pre>
 * packet gPtp_FollowUp
 * {
 *     omnetpp::SimTime originTimestamp;
 *     omnetpp::SimTime ratio;//ratio也用SimTime是因为double的精度不够高
 *     omnetpp::SimTime correctionField;
 * }
 * </pre>
 */
class gPtp_FollowUp : public ::omnetpp::cPacket
{
  protected:
    omnetpp::SimTime originTimestamp;
    omnetpp::SimTime ratio;
    omnetpp::SimTime correctionField;

  private:
    void copy(const gPtp_FollowUp& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const gPtp_FollowUp&);

  public:
    gPtp_FollowUp(const char *name=nullptr, int kind=0);
    gPtp_FollowUp(const gPtp_FollowUp& other);
    virtual ~gPtp_FollowUp();
    gPtp_FollowUp& operator=(const gPtp_FollowUp& other);
    virtual gPtp_FollowUp *dup() const {return new gPtp_FollowUp(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b);

    // field getter/setter methods
    virtual omnetpp::SimTime& getOriginTimestamp();
    virtual const omnetpp::SimTime& getOriginTimestamp() const {return const_cast<gPtp_FollowUp*>(this)->getOriginTimestamp();}
    virtual void setOriginTimestamp(const omnetpp::SimTime& originTimestamp);
    virtual omnetpp::SimTime& getRatio();
    virtual const omnetpp::SimTime& getRatio() const {return const_cast<gPtp_FollowUp*>(this)->getRatio();}
    virtual void setRatio(const omnetpp::SimTime& ratio);
    virtual omnetpp::SimTime& getCorrectionField();
    virtual const omnetpp::SimTime& getCorrectionField() const {return const_cast<gPtp_FollowUp*>(this)->getCorrectionField();}
    virtual void setCorrectionField(const omnetpp::SimTime& correctionField);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const gPtp_FollowUp& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, gPtp_FollowUp& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>gPtpPacket.msg:29</tt> by nedtool.
 * <pre>
 * packet gPtp_PdelayReq
 * {
 * }
 * </pre>
 */
class gPtp_PdelayReq : public ::omnetpp::cPacket
{
  protected:

  private:
    void copy(const gPtp_PdelayReq& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const gPtp_PdelayReq&);

  public:
    gPtp_PdelayReq(const char *name=nullptr, int kind=0);
    gPtp_PdelayReq(const gPtp_PdelayReq& other);
    virtual ~gPtp_PdelayReq();
    gPtp_PdelayReq& operator=(const gPtp_PdelayReq& other);
    virtual gPtp_PdelayReq *dup() const {return new gPtp_PdelayReq(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b);

    // field getter/setter methods
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const gPtp_PdelayReq& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, gPtp_PdelayReq& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>gPtpPacket.msg:33</tt> by nedtool.
 * <pre>
 * packet gPtp_PdelayResp
 * {
 *     omnetpp::SimTime sentTime;
 *     omnetpp::SimTime requestReceiptTimestamp;
 * }
 * </pre>
 */
class gPtp_PdelayResp : public ::omnetpp::cPacket
{
  protected:
    omnetpp::SimTime sentTime;
    omnetpp::SimTime requestReceiptTimestamp;

  private:
    void copy(const gPtp_PdelayResp& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const gPtp_PdelayResp&);

  public:
    gPtp_PdelayResp(const char *name=nullptr, int kind=0);
    gPtp_PdelayResp(const gPtp_PdelayResp& other);
    virtual ~gPtp_PdelayResp();
    gPtp_PdelayResp& operator=(const gPtp_PdelayResp& other);
    virtual gPtp_PdelayResp *dup() const {return new gPtp_PdelayResp(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b);

    // field getter/setter methods
    virtual omnetpp::SimTime& getSentTime();
    virtual const omnetpp::SimTime& getSentTime() const {return const_cast<gPtp_PdelayResp*>(this)->getSentTime();}
    virtual void setSentTime(const omnetpp::SimTime& sentTime);
    virtual omnetpp::SimTime& getRequestReceiptTimestamp();
    virtual const omnetpp::SimTime& getRequestReceiptTimestamp() const {return const_cast<gPtp_PdelayResp*>(this)->getRequestReceiptTimestamp();}
    virtual void setRequestReceiptTimestamp(const omnetpp::SimTime& requestReceiptTimestamp);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const gPtp_PdelayResp& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, gPtp_PdelayResp& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>gPtpPacket.msg:39</tt> by nedtool.
 * <pre>
 * packet gPtp_PdelayRespFollowUp
 * {
 * }
 * </pre>
 */
class gPtp_PdelayRespFollowUp : public ::omnetpp::cPacket
{
  protected:

  private:
    void copy(const gPtp_PdelayRespFollowUp& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const gPtp_PdelayRespFollowUp&);

  public:
    gPtp_PdelayRespFollowUp(const char *name=nullptr, int kind=0);
    gPtp_PdelayRespFollowUp(const gPtp_PdelayRespFollowUp& other);
    virtual ~gPtp_PdelayRespFollowUp();
    gPtp_PdelayRespFollowUp& operator=(const gPtp_PdelayRespFollowUp& other);
    virtual gPtp_PdelayRespFollowUp *dup() const {return new gPtp_PdelayRespFollowUp(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b);

    // field getter/setter methods
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const gPtp_PdelayRespFollowUp& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, gPtp_PdelayRespFollowUp& obj) {obj.parsimUnpack(b);}


#endif // ifndef __GPTPPACKET_M_H
