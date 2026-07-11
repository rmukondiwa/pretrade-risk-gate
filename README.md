# Designing an Order Flow Rate Limiter / Risk Gate

This document describes the design of an order flow rate limiter / risk gate for a trading system. The goal of this mechanism is to prevent excessive order flow that could lead to market manipulation, system overload, or other general security risks.

## Where to Implement the Rate Limiter
The rate limiter can be implemented at various points in the order processing pipeline, including:
1. **Client-Side**: Can help reduce the load of the server by preventing a bad actor from sending too many requests.
2. **Server-Side**: Allows for centralized control and monitoring of order flow, making it easier to enforce limits and detect patterns of abuse.

## Design Requirements
1. **Configurable Limits**: It should allow for configurable limits based on various parameters such as:
    - Number of orders per second/minute/hour. (e.g., 100 orders per minute)
    - Total order value per time unit. (e.g., $1,000,000 per hour)
    - Number of orders per trading symbol. (e.g., 50 orders per symbol per minute)
2. **User-Based Limits**: Different limits for different users or groups of users (e.g., retail vs institutional clients).
3. **Real-Time Monitoring**: The system should provide real-time monitoring and alerting capabilities to detect when limits are being approached or exceeded.
4. **Graceful Handling**: When limits are exceeded, the system should handle it gracefully by rejecting orders with appropriate error messages and logging the incidents for further analysis.
5. **Scalability**: The rate limiter should be designed to scale with the growth of the user base and order volume.
6. **Low Latency**: The implementation should minimize latency to ensure that it does not negatively impact the user experience.

# Implementation Approaches
## Naive Approach:
** Counter-Based With Fixed Window**: This approach uses a simple counter to track the number of orders within a fixed time window. For example, if the limit is 100 orders per minute, the counter resets every minute. This approach is easy to implement but can lead to burstiness at the edges of the time window. A user could send 100 orders at the end of one minute and then immediately send another 100 orders at the start of the next.

## Better Approach: 
** Token Bucket Algorithm **: This algorithm allows for a more flexible rate limiting mechanism. It uses a bucket that fills with tokens at a constant rate. Each order requires a token to be processed and if the bucket is empty, the order is rejected. This approach allows for bursts of orders while still enforcing an average rate over time.

# Low Latency Considerations
## The hot path must never stall or block.

1. No allocation: No new/malloc. Everything is pre-allocated: object pools, fixed-size buffers, etc.
2. No locks: It must be single threaded. All state on one thread, nothing to lock. Threads communicate via lock free ring buffers, not shared mutexes.
3. No syscalls: No file I/O, no network I/O, no time calls
4. Integer math only: No floating point math, no division, no modulus. Only addition, subtraction, bit shifts, and bitwise operations.